# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'includes': [
    'base.gypi',
  ],
  'targets': [
    {
      'target_name': 'base',
      'type': '<(component)',
      'variables': {
        'base_target': 1,
        'enable_wexit_time_destructors': 1,
        'optimize': 'max',
      },
      'dependencies': [
        '<(DEPTH)/nb/nb.gyp:nb',
        '<(DEPTH)/starboard/client_porting/eztime/eztime.gyp:eztime',
        '<(DEPTH)/starboard/starboard.gyp:starboard',
        '<(DEPTH)/testing/gtest.gyp:gtest_prod',
        '<(DEPTH)/third_party/modp_b64/modp_b64.gyp:modp_b64',
        'allocator/allocator.gyp:allocator_extension_thunks',
        'base_static',
        'third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
      ],
      'sources': [
        'auto_reset.h',
        'base_paths_starboard.cc',
        'event_recorder.h',
        'event_recorder_stubs.cc',
        'md5.cc',
        'md5.h',
        'message_pump_observer.h',
        'metrics/field_trial.cc',
        'metrics/field_trial.h',
        'sync_socket.h',
        'test/test_file_util_starboard.cc',
        'third_party/nspr/prcpucfg.h',
        'third_party/nspr/prtypes.h',
      ],
    },
    {
      'target_name': 'base_i18n',
      'type': '<(component)',
      'variables': {
        'enable_wexit_time_destructors': 1,
        'optimize': 'max',
      },
      'dependencies': [
        'base',
        'third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
        '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
        '<(DEPTH)/starboard/starboard.gyp:starboard',
        '<(DEPTH)/starboard/client_porting/icu_init/icu_init.gyp:icu_init',
      ],
      'export_dependent_settings': [
        'base',
      ],
      'defines': [
        'BASE_I18N_IMPLEMENTATION',
      ],
      'sources': [
        'i18n/base_i18n_export.h',
        'i18n/bidi_line_iterator.cc',
        'i18n/bidi_line_iterator.h',
        'i18n/break_iterator.cc',
        'i18n/break_iterator.h',
        'i18n/char_iterator.cc',
        'i18n/char_iterator.h',
        'i18n/case_conversion.cc',
        'i18n/case_conversion.h',
        'i18n/file_util_icu.cc',
        'i18n/file_util_icu.h',
        'i18n/icu_encoding_detection.cc',
        'i18n/icu_encoding_detection.h',
        'i18n/icu_string_conversions.cc',
        'i18n/icu_string_conversions.h',
        'i18n/icu_util.cc',
        'i18n/icu_util.h',
        'i18n/number_formatting.cc',
        'i18n/number_formatting.h',
        'i18n/rtl.cc',
        'i18n/rtl.h',
        'i18n/string_search.cc',
        'i18n/string_search.h',
        'i18n/time_formatting.cc',
        'i18n/time_formatting.h',
      ],
    },
    {
      'target_name': 'base_prefs',
      'type': '<(component)',
      'variables': {
        'enable_wexit_time_destructors': 1,
        'optimize': 'max',
      },
      'dependencies': [
        'base',
      ],
      'export_dependent_settings': [
        'base',
      ],
      'defines': [
        'BASE_PREFS_IMPLEMENTATION',
      ],
      'sources': [
        'prefs/default_pref_store.cc',
        'prefs/default_pref_store.h',
        'prefs/overlay_user_pref_store.cc',
        'prefs/overlay_user_pref_store.h',
        'prefs/persistent_pref_store.h',
        'prefs/pref_observer.h',
        'prefs/pref_notifier.h',
        'prefs/pref_store.cc',
        'prefs/pref_store.h',
        'prefs/pref_value_map.cc',
        'prefs/pref_value_map.h',
        'prefs/public/pref_change_registrar.cc',
        'prefs/public/pref_change_registrar.h',
        'prefs/public/pref_member.cc',
        'prefs/public/pref_member.h',
        'prefs/public/pref_service_base.h',
        'prefs/value_map_pref_store.cc',
        'prefs/value_map_pref_store.h',
      ],
    },
    {
      # This is the subset of files from base that should not be used with a
      # dynamic library. Note that this library cannot depend on base because
      # base depends on base_static.
      'target_name': 'base_static',
      'type': 'static_library',
      'variables': {
        'enable_wexit_time_destructors': 1,
        'optimize': 'max',
      },
      'toolsets': ['host', 'target'],
      'sources': [
        'base_switches.cc',
        'base_switches.h',
      ],
      'include_dirs': [
        '<(DEPTH)',
      ],
    },
    # Include this target for a main() function that simply instantiates
    # and runs a base::TestSuite.
    {
      'target_name': 'run_all_unittests',
      'type': 'static_library',
      'dependencies': [
        'test_support_base',
      ],
      'sources': [
        'test/run_all_unittests.cc',
      ],
    },
    {
      'target_name': 'base_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        # Tests.
        'at_exit_unittest.cc',
        'atomicops_unittest.cc',
        'base64_unittest.cc',
        'bind_helpers_unittest.cc',
        'bind_unittest.cc',
        'bind_unittest.nc',
        'bits_unittest.cc',
        'build_time_unittest.cc',
        'callback_unittest.cc',
        'callback_unittest.nc',
        'cancelable_callback_unittest.cc',
        'circular_buffer_shell_unittest.cc',
        'command_line_unittest.cc',
        'containers/linked_list_unittest.cc',
        'containers/mru_cache_unittest.cc',
        'containers/small_map_unittest.cc',
        'containers/stack_container_unittest.cc',
        'cpu_unittest.cc',
        'debug/leak_tracker_unittest.cc',
        'debug/stack_trace_unittest.cc',
        'debug/trace_event_unittest.cc',
        'debug/trace_event_unittest.h',
        'debug/trace_event_win_unittest.cc',
        'file_path_unittest.cc',
        'file_util_proxy_unittest.cc',
        'file_util_unittest.cc',
        'file_version_info_unittest.cc',
        'files/dir_reader_posix_unittest.cc',
        'files/scoped_temp_dir_unittest.cc',
        'gmock_unittest.cc',
        'guid_unittest.cc',
        'hi_res_timer_manager_unittest.cc',
        'i18n/break_iterator_unittest.cc',
        'i18n/case_conversion_unittest.cc',
        'i18n/char_iterator_unittest.cc',
        'i18n/file_util_icu_unittest.cc',
        'i18n/icu_string_conversions_unittest.cc',
        'i18n/number_formatting_unittest.cc',
        'i18n/rtl_unittest.cc',
        'i18n/string_search_unittest.cc',
        'i18n/time_formatting_unittest.cc',
        'id_map_unittest.cc',
        'json/json_parser_unittest.cc',
        'json/json_reader_unittest.cc',
        'json/json_value_converter_unittest.cc',
        'json/json_value_serializer_unittest.cc',
        'json/json_writer_unittest.cc',
        'json/string_escape_unittest.cc',
        'lazy_instance_unittest.cc',
        'logging_unittest.cc',
        'md5_unittest.cc',
        'memory/aligned_memory_unittest.cc',
        'memory/linked_ptr_unittest.cc',
        'memory/ref_counted_memory_unittest.cc',
        'memory/ref_counted_unittest.cc',
        'memory/scoped_nsobject_unittest.mm',
        'memory/scoped_ptr_unittest.cc',
        'memory/scoped_ptr_unittest.nc',
        'memory/scoped_vector_unittest.cc',
        'memory/singleton_unittest.cc',
        'memory/weak_ptr_unittest.cc',
        'memory/weak_ptr_unittest.nc',
        'message_loop_proxy_impl_unittest.cc',
        'message_loop_proxy_unittest.cc',
        'message_loop_unittest.cc',
        'metrics/bucket_ranges_unittest.cc',
        'metrics/histogram_unittest.cc',
        'metrics/sample_map_unittest.cc',
        'metrics/sample_vector_unittest.cc',
        'metrics/sparse_histogram_unittest.cc',
        'metrics/statistics_recorder_unittest.cc',
        'object_tracker.h',
        'observer_list_unittest.cc',
        'optional_unittest.cc',
        'path_service_unittest.cc',
        'pickle_unittest.cc',
        'platform_file_unittest.cc',
        'pr_time_unittest.cc',
        'profiler/tracked_time_unittest.cc',
        'rand_util_unittest.cc',
        'scoped_observer.h',
        'sha1_unittest.cc',
        'state_machine_shell_unittest.cc',
        'stl_util_unittest.cc',
        'string16_unittest.cc',
        'string_number_conversions_unittest.cc',
        'string_piece_unittest.cc',
        'string_split_unittest.cc',
        'string_tokenizer_unittest.cc',
        'string_util_unittest.cc',
        'stringize_macros_unittest.cc',
        'stringprintf_unittest.cc',
        'synchronization/cancellation_flag_unittest.cc',
        'synchronization/condition_variable_unittest.cc',
        'synchronization/lock_unittest.cc',
        'synchronization/waitable_event_unittest.cc',
        'sys_info_unittest.cc',
        'sys_string_conversions_unittest.cc',
        'system_monitor/system_monitor_unittest.cc',
        'task_runner_util_unittest.cc',
        'template_util_unittest.cc',
        'test/sequenced_worker_pool_owner.cc',
        'test/sequenced_worker_pool_owner.h',
        'test/time_helpers.cc',
        'test/time_helpers.h',
        'test/trace_event_analyzer_unittest.cc',
        'threading/non_thread_safe_unittest.cc',
        'threading/platform_thread_unittest.cc',
        'threading/sequenced_worker_pool_unittest.cc',
        'threading/simple_thread_unittest.cc',
        'threading/thread_checker_unittest.cc',
        'threading/thread_collision_warner_unittest.cc',
        'threading/thread_local_storage_unittest.cc',
        'threading/thread_local_unittest.cc',
        'threading/thread_unittest.cc',
        'threading/watchdog_unittest.cc',
        'threading/worker_pool_unittest.cc',
        'time_unittest.cc',
        'time_win_unittest.cc',
        'timer_unittest.cc',
        'tools_sanity_unittest.cc',
        'tracked_objects_unittest.cc',
        'tuple_unittest.cc',
        'utf_offset_string_conversions_unittest.cc',
        'utf_string_conversions_unittest.cc',
        'values_unittest.cc',
        'version_unittest.cc',
        'vlog_unittest.cc',
      ],
      'dependencies': [
        'base',
        'base_i18n',
        'base_static',
        'run_all_unittests',
        'test_support_base',
        'third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '../testing/gmock.gyp:gmock',
        '../testing/gtest.gyp:gtest',
        '../third_party/icu/icu.gyp:icui18n',
        '../third_party/icu/icu.gyp:icuuc',
      ],
      'variables': {
         # TODO(ajwong): Is there a way to autodetect this?
        'module_dir': 'base'
      },
      'actions': [
        {
          'action_name': 'copy_test_data',
          'variables': {
            'input_files': [
              'data/json',
              'data/file_util_unittest',
            ],
            'output_dir': 'base/data',
          },
          'includes': [ '../cobalt/build/copy_test_data.gypi' ],
        },
      ],
      'msvs_disabled_warnings': [
        # forcing value to bool 'true' or 'false' (performance warning)
        4800,
        # Destructor is explicitly deleted.
        4624,
      ],
    },
    {
      'target_name': 'test_support_base',
      'type': 'static_library',
      'dependencies': [
        '<(DEPTH)/testing/gmock.gyp:gmock',
        '<(DEPTH)/testing/gtest.gyp:gtest',
        'base',
        'base_static',
        'base_i18n',
      ],
      'export_dependent_settings': [
        'base',
      ],
      'sources': [
        'perftimer_starboard.cc',
        'test/main_hook.cc',
        'test/main_hook.h',
        'test/mock_devices_changed_observer.cc',
        'test/mock_devices_changed_observer.h',
        'test/mock_time_provider.cc',
        'test/mock_time_provider.h',
        'test/perf_test_suite.cc',
        'test/perf_test_suite.h',
        'test/scoped_path_override.cc',
        'test/scoped_path_override.h',
        'test/sequenced_task_runner_test_template.cc',
        'test/sequenced_task_runner_test_template.h',
        'test/task_runner_test_template.cc',
        'test/task_runner_test_template.h',
        'test/test_file_util.h',
        'test/test_suite.cc',
        'test/test_suite.h',
        'test/test_switches.cc',
        'test/test_switches.h',
        'test/test_timeouts.cc',
        'test/test_timeouts.h',
        'test/thread_test_helper.cc',
        'test/thread_test_helper.h',
        'test/trace_event_analyzer.cc',
        'test/trace_event_analyzer.h',
        'test/values_test_util.cc',
        'test/values_test_util.h',
      ],
    },
    {
      'target_name': 'base_unittests_deploy',
      'type': 'none',
      'dependencies': [
        'base_unittests',
      ],
      'variables': {
        'executable_name': 'base_unittests',
      },
      'includes': [ '../starboard/build/deploy.gypi' ],
    },
  ],
}
