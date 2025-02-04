# Copyright (C) 2011, Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import copy
import logging

from webkitpy.common.memoized import memoized
from webkitpy.layout_tests.controllers.test_result_writer import baseline_name as get_baseline_name
from webkitpy.layout_tests.models.testharness_results import is_all_pass_testharness_result

_log = logging.getLogger(__name__)

ALL_PASS = '<PASS> testharness.js'


class BaselineOptimizer(object):

    def __init__(self, host, default_port, port_names):
        self._filesystem = host.filesystem
        self._default_port = default_port
        self._ports = {}
        for port_name in port_names:
            self._ports[port_name] = host.port_factory.get(port_name)

        self._layout_tests_dir = default_port.layout_tests_dir()
        self._parent_of_tests = self._filesystem.dirname(self._layout_tests_dir)
        self._layout_tests_dir_name = self._filesystem.relpath(
            self._layout_tests_dir, self._parent_of_tests)

        # Only used by unit tests.
        self.new_results_by_directory = []

    def optimize(self, test_name, suffix):
        # A visualization of baseline fallback:
        # https://docs.google.com/drawings/d/13l3IUlSE99RoKjDwEWuY1O77simAhhF6Wi0fZdkSaMA/
        # The full document with more details:
        # https://chromium.googlesource.com/chromium/src/+/master/docs/testing/layout_test_baseline_fallback.md
        # The virtual and non-virtual subtrees are identical, with the virtual
        # root being the special node having multiple parents and connecting the
        # two trees. We patch the virtual subtree to cut its dependencies on the
        # non-virtual one and optimze the two independently. Finally, we treat
        # the virtual subtree specially to remove any duplication between the
        # two subtrees.
        assert not suffix.startswith('.')
        baseline_name = get_baseline_name(self._filesystem, test_name, suffix)
        non_virtual_baseline_name = self._virtual_base(baseline_name)
        succeeded = True
        if non_virtual_baseline_name:
            # The baseline belongs to a virtual suite.
            _log.debug('Optimizing virtual fallback path.')
            self._patch_virtual_subtree(test_name, suffix, baseline_name)
            succeeded &= self._optimize_subtree(baseline_name)
            self._optimize_virtual_root(test_name, suffix, baseline_name)
        else:
            # The given baseline is already non-virtual.
            non_virtual_baseline_name = baseline_name

        _log.debug('Optimizing non-virtual fallback path.')
        succeeded &= self._optimize_subtree(non_virtual_baseline_name)
        self._remove_all_pass_testharness_result_at_root(non_virtual_baseline_name)

        if not succeeded:
            _log.error('Heuristics failed to optimize %s', baseline_name)
        return succeeded

    def write_by_directory(self, results_by_directory, writer, indent):
        """Logs results_by_directory in a pretty format."""
        for path in sorted(results_by_directory):
            writer('%s%s: %s' % (indent, self._platform(path), results_by_directory[path][0:6]))

    def read_results_by_directory(self, baseline_name):
        """Reads the baselines with the given file name in all directories.

        Returns:
            A dict from directory names to the digest of file content.
        """
        results_by_directory = {}
        directories = set()
        for port in self._ports.values():
            directories.update(set(self._relative_baseline_search_path(port)))

        for directory in directories:
            path = self._join_directory(directory, baseline_name)
            if self._filesystem.exists(path):
                results_by_directory[directory] = self._digest_result(path)
        return results_by_directory

    def _optimize_subtree(self, baseline_name):
        basename = self._filesystem.basename(baseline_name)
        results_by_directory, new_results_by_directory = self._find_optimal_result_placement(baseline_name)

        if new_results_by_directory == results_by_directory:
            if new_results_by_directory:
                _log.debug('  %s: (already optimal)', basename)
                self.write_by_directory(results_by_directory, _log.debug, '    ')
            else:
                _log.debug('  %s: (no baselines found)', basename)
            # This is just used for unit tests.
            # Intentionally set it to the old data if we don't modify anything.
            self.new_results_by_directory.append(results_by_directory)
            return True

        # Check if the results before and after optimization are equivalent.
        if (self._results_by_port_name(results_by_directory) !=
                self._results_by_port_name(new_results_by_directory)):
            # This really should never happen. Just a sanity check to make
            # sure the script fails in the case of bugs instead of committing
            # incorrect baselines.
            _log.error('  %s: optimization failed', basename)
            self.write_by_directory(results_by_directory, _log.warning, '      ')
            return False

        _log.debug('  %s:', basename)
        _log.debug('    Before: ')
        self.write_by_directory(results_by_directory, _log.debug, '      ')
        _log.debug('    After: ')
        self.write_by_directory(new_results_by_directory, _log.debug, '      ')

        self._move_baselines(baseline_name, results_by_directory, new_results_by_directory)
        return True

    def _move_baselines(self, baseline_name, results_by_directory, new_results_by_directory):
        data_for_result = {}
        for directory, result in results_by_directory.items():
            if result not in data_for_result:
                source = self._join_directory(directory, baseline_name)
                data_for_result[result] = self._filesystem.read_binary_file(source)

        fs_files = []
        for directory, result in results_by_directory.items():
            if new_results_by_directory.get(directory) != result:
                file_name = self._join_directory(directory, baseline_name)
                if self._filesystem.exists(file_name):
                    fs_files.append(file_name)

        if fs_files:
            _log.debug('    Deleting (file system):')
            for platform_dir in sorted(self._platform(filename) for filename in fs_files):
                _log.debug('      ' + platform_dir)
            for filename in fs_files:
                self._filesystem.remove(filename)
        else:
            _log.debug('    (Nothing to delete)')

        file_names = []
        for directory, result in new_results_by_directory.items():
            if results_by_directory.get(directory) != result:
                destination = self._join_directory(directory, baseline_name)
                self._filesystem.maybe_make_directory(self._filesystem.split(destination)[0])
                self._filesystem.write_binary_file(destination, data_for_result[result])
                file_names.append(destination)

        if file_names:
            _log.debug('    Adding:')
            for platform_dir in sorted(self._platform(filename) for filename in file_names):
                _log.debug('      ' + platform_dir)
        else:
            _log.debug('    (Nothing to add)')

    def _platform(self, filename):
        """Guesses the platform from a path (absolute or relative).

        Returns:
            The platform name, or '(generic)' if unable to make a guess.
        """
        platform_dir = self._layout_tests_dir_name + self._filesystem.sep + 'platform' + self._filesystem.sep
        if filename.startswith(platform_dir):
            return filename.replace(platform_dir, '').split(self._filesystem.sep)[0]
        platform_dir = self._filesystem.join(self._parent_of_tests, platform_dir)
        if filename.startswith(platform_dir):
            return filename.replace(platform_dir, '').split(self._filesystem.sep)[0]
        return '(generic)'

    def _port_from_baseline_dir(self, baseline_dir):
        """Returns a Port object from the given baseline directory."""
        baseline_dir = self._filesystem.basename(baseline_dir)
        for port in self._ports.values():
            if self._filesystem.basename(port.baseline_version_dir()) == baseline_dir:
                return port
        raise Exception('Failed to find port for primary baseline %s.' % baseline_dir)

    def _walk_immediate_predecessors_of_virtual_root(self, test_name, suffix, baseline_name, worker_func):
        """Maps a function onto each immediate predecessor of the virtual root.

        For each immediate predecessor, we call
            worker_func(virtual_baseline, non_virtual_fallback)
        where the two arguments are the absolute paths to the virtual platform
        baseline and the non-virtual fallback respectively.
        """
        actual_test_name = self._virtual_base(test_name)
        assert actual_test_name, '%s is not a virtual test.' % test_name

        # TODO(robertma): Suffixes expected by test_result_writer.baseline_name
        # and port.expected_filename are inconsitent; the former does not
        # expect the extension dot, while the latter does.
        dotted_suffix = '.' + suffix

        for directory in self._directories_immediately_preceding_root():
            port = self._port_from_baseline_dir(directory)
            virtual_baseline = self._join_directory(directory, baseline_name)
            # return_default=False mandates expected_filename() to return None
            # instead of a non-existing generic path when nothing is found.
            non_virtual_fallback = port.expected_filename(actual_test_name, dotted_suffix, return_default=False)
            if not non_virtual_fallback:
                # Unable to find a non-virtual fallback baseline, skipping.
                continue
            worker_func(virtual_baseline, non_virtual_fallback)

    def _patch_virtual_subtree(self, test_name, suffix, baseline_name):
        # Ensure all immediate predecessors of the root have a baseline for this
        # virtual suite so that the virtual subtree can be treated completely
        # independently. If an immediate predecessor is missing a baseline, find
        # its non-virtual fallback and copy over.
        _log.debug('Copying non-virtual baselines to the virtual subtree to make it independent.')
        virtual_root_baseline_path = self._filesystem.join(self._layout_tests_dir, baseline_name)
        if self._filesystem.exists(virtual_root_baseline_path):
            return

        def patcher(virtual_baseline, non_virtual_fallback):
            if not self._filesystem.exists(virtual_baseline):
                _log.debug('    Copying (file system): %s -> %s.', non_virtual_fallback, virtual_baseline)
                self._filesystem.maybe_make_directory(self._filesystem.split(virtual_baseline)[0])
                self._filesystem.copyfile(non_virtual_fallback, virtual_baseline)

        self._walk_immediate_predecessors_of_virtual_root(test_name, suffix, baseline_name, patcher)

    def _optimize_virtual_root(self, test_name, suffix, baseline_name):
        virtual_root_baseline_path = self._filesystem.join(self._layout_tests_dir, baseline_name)
        if self._filesystem.exists(virtual_root_baseline_path):
            _log.debug('Virtual root baseline found. Checking if we can remove it.')
            self._try_to_remove_virtual_root(baseline_name, virtual_root_baseline_path)
        else:
            _log.debug('Virtual root baseline not found. Searching for virtual baselines redundant with non-virtual ones.')
            self._unpatch_virtual_subtree(test_name, suffix, baseline_name)

    def _try_to_remove_virtual_root(self, baseline_name, virtual_root_baseline_path):
        # See if all the successors of the virtual root (i.e. all non-virtual
        # platforms) have the same baseline as the virtual root. If so, the
        # virtual root is redundant and can be safely removed.
        virtual_root_digest = self._digest_result(virtual_root_baseline_path)

        # Read the base (non-virtual) results.
        results_by_directory = self.read_results_by_directory(self._virtual_base(baseline_name))
        # Since implicit_all_pass defaults to True, we can also optimize away
        # redundant all-PASS testharness.js results at the virtual root.
        results_by_port_name = self._results_by_port_name(results_by_directory)

        for port_name in self._ports.keys():
            assert port_name in results_by_port_name
            if results_by_port_name[port_name] != virtual_root_digest:
                return

        _log.debug('Deleting redundant virtual root baseline.')
        _log.debug('    Deleting (file system): ' + virtual_root_baseline_path)
        self._filesystem.remove(virtual_root_baseline_path)

    def _unpatch_virtual_subtree(self, test_name, suffix, baseline_name):
        # Check all immediate predecessors of the virtual root and delete those
        # duplicate with their non-virtual fallback, essentially undoing some
        # of the work done in _patch_virtual_subtree.
        def unpatcher(virtual_baseline, non_virtual_fallback):
            if self._filesystem.exists(virtual_baseline) and \
                    self._digest_result(virtual_baseline) == self._digest_result(non_virtual_fallback):
                _log.debug('    Deleting (file system): %s (redundant with %s).', virtual_baseline, non_virtual_fallback)
                self._filesystem.remove(virtual_baseline)
        self._walk_immediate_predecessors_of_virtual_root(test_name, suffix, baseline_name, unpatcher)

    def _baseline_root(self):
        """Returns the name of the root (generic) baseline directory."""
        return self._layout_tests_dir_name

    def _baseline_search_path(self, port):
        """Returns the baseline search path (a list of absolute paths) of the
        given port."""
        return port.baseline_search_path()

    @memoized
    def _relative_baseline_search_path(self, port):
        """Returns a list of paths to check for baselines in order.

        The generic baseline path is appended to the list. All paths are
        relative to the parent of the test directory.
        """
        baseline_search_path = self._baseline_search_path(port)
        relative_paths = [self._filesystem.relpath(path, self._parent_of_tests) for path in baseline_search_path]
        relative_baseline_root = self._baseline_root()
        return relative_paths + [relative_baseline_root]

    def _virtual_base(self, baseline_name):
        """Returns the base (non-virtual) version of baseline_name, or None if
        baseline_name is not virtual."""
        # Note: port.lookup_virtual_test_base in fact expects a test_name,
        # but baseline_name also works here.
        return self._default_port.lookup_virtual_test_base(baseline_name)

    def _join_directory(self, directory, baseline_name):
        """Returns the absolute path to the baseline in the given directory."""
        return self._filesystem.join(self._parent_of_tests, directory, baseline_name)

    def _results_by_port_name(self, results_by_directory, implicit_all_pass=True):
        """Transforms a by-directory result dict to by-port-name.

        The method mimicks the baseline search behaviour, i.e. results[port] is
        the first baseline found on the baseline search path of the port.

        Args:
            results_by_directory: A dictionary returned by read_results_by_directory().
            implicit_all_pass: If True, ports with no baselines found will have
                all-PASS testharness.js results, which matches the real-world
                behaviour of run-webkit-tests for testharness.js tests.
                Otherwise, such ports will not be found in the dict.

        Returns:
            A dictionary mapping port names to their baselines.
        """
        results_by_port_name = {}
        for port_name, port in self._ports.items():
            for directory in self._relative_baseline_search_path(port):
                if directory in results_by_directory:
                    results_by_port_name[port_name] = results_by_directory[directory]
                    break
            if port_name not in results_by_port_name and implicit_all_pass:
                results_by_port_name[port_name] = ALL_PASS
        return results_by_port_name

    @memoized
    def _directories_immediately_preceding_root(self):
        """Returns a list of directories immediately preceding the root on
        search paths."""
        directories = set()
        for port in self._ports.values():
            directory = self._filesystem.relpath(self._baseline_search_path(port)[-1], self._parent_of_tests)
            directories.add(directory)
        return frozenset(directories)

    def _optimize_result_for_root(self, new_results_by_directory):
        # The root directory (i.e. LayoutTests) is the only one not
        # corresponding to a specific platform. As such, baselines in
        # directories that immediately precede the root on search paths may
        # be promoted up if they are all the same.
        # Example: if win and mac have the same baselines, then they can be
        # promoted up to be the root baseline.
        # All other baselines can only be removed if they're redundant with a
        # baseline later on the search path. They can never be promoted up.
        immediately_preceding_root = self._directories_immediately_preceding_root()

        shared_result = None
        root_baseline_unused = False
        for directory in immediately_preceding_root:
            this_result = new_results_by_directory.get(directory)

            # If any of these directories don't have a baseline, there's no optimization we can do.
            if not this_result:
                return

            if not shared_result:
                shared_result = this_result
            elif shared_result != this_result:
                root_baseline_unused = True

        baseline_root = self._baseline_root()

        # The root baseline is unused if all the directories immediately preceding the root
        # have a baseline, but have different baselines, so the baselines can't be promoted up.
        if root_baseline_unused:
            if baseline_root in new_results_by_directory:
                del new_results_by_directory[baseline_root]
            return

        new_results_by_directory[baseline_root] = shared_result
        for directory in immediately_preceding_root:
            del new_results_by_directory[directory]

    def _find_optimal_result_placement(self, baseline_name):
        results_by_directory = self.read_results_by_directory(baseline_name)
        # Set implicit_all_pass=False so that we won't get non-existent all-PASS
        # testharness.js results, which would break _find_in_search_path.
        results_by_port_name = self._results_by_port_name(results_by_directory, implicit_all_pass=False)

        new_results_by_directory = self._remove_redundant_results(
            results_by_directory, results_by_port_name)
        self._optimize_result_for_root(new_results_by_directory)

        return results_by_directory, new_results_by_directory

    def _remove_redundant_results(self, results_by_directory, results_by_port_name):
        # For every port, traverse its search path in the fallback order (from
        # specific to generic). Remove duplicate baselines until a different
        # baseline is found (or the root is reached), i.e., keep the most
        # generic one among duplicate baselines.
        new_results_by_directory = copy.copy(results_by_directory)
        for port_name, port in self._ports.items():
            current_result = results_by_port_name.get(port_name)

            # This happens if we're missing baselines for a port.
            if not current_result:
                continue

            search_path = self._relative_baseline_search_path(port)
            current_index, current_directory = self._find_in_search_path(search_path, current_result, new_results_by_directory)
            found_different_result = False
            for index in range(current_index + 1, len(search_path)):
                new_directory = search_path[index]
                if new_directory not in new_results_by_directory:
                    # No baseline in this directory.
                    continue
                elif new_results_by_directory[new_directory] == current_result:
                    # The baseline in current_directory is redundant with the
                    # baseline in new_directory which is later in the search
                    # path. Remove the earlier one and point current to new.
                    if current_directory in new_results_by_directory:
                        del new_results_by_directory[current_directory]
                        current_directory = new_directory
                else:
                    # A different result is found, so stop.
                    found_different_result = True
                    break

            # If we did not find a different fallback and current_result is an
            # all-PASS testharness.js result, we can safely remove it, as it is
            # the implicit result.
            # Note that we do not remove the generic all-PASS result here.
            # Roots (virtual and non-virtual) are treated specially later.
            if (not found_different_result and current_result == ALL_PASS
                    and current_directory != self._baseline_root()
                    and current_directory in new_results_by_directory):
                del new_results_by_directory[current_directory]

        return new_results_by_directory

    def _find_in_search_path(self, search_path, current_result, results_by_directory):
        """Finds the index and the directory of a result on a search path."""
        for index, directory in enumerate(search_path):
            if directory in results_by_directory and (results_by_directory[directory] == current_result):
                return index, directory
        assert False, 'result %s not found in search path %s, %s' % (current_result, search_path, results_by_directory)

    def _remove_all_pass_testharness_result_at_root(self, baseline_name):
        """Removes the all-PASS testharness.js result at the non-virtual root."""
        assert not self._virtual_base(baseline_name), 'A virtual baseline is passed in.'
        path = self._join_directory(self._baseline_root(), baseline_name)
        if self._filesystem.exists(path) and self._is_all_pass_testharness_result(path):
            _log.debug('Deleting redundant all-PASS root baseline.')
            _log.debug('    Deleting (file system): ' + path)
            self._filesystem.remove(path)

    def _digest_result(self, path):
        """Digests a result file into a string suitable for comparison.

        Args:
            An absolute path.

        Returns:
            In most cases, the SHA1 sum of the content is returned, but if the
            file is an all-PASS testharness.js result, ALL_PASS is returned.
        """
        # Unfortunately, we may read the file twice, once in text mode and once
        # in binary mode.
        if self._is_all_pass_testharness_result(path):
            return ALL_PASS
        return self._filesystem.sha1(path)

    def _is_all_pass_testharness_result(self, path):
        """Checks if a baseline is an all-PASS testharness.js result."""
        # TODO(robertma): Find an appropriate constant for this (or make one).
        if not path.endswith('-expected.txt'):
            return False
        content = self._filesystem.read_text_file(path)
        return is_all_pass_testharness_result(content)
