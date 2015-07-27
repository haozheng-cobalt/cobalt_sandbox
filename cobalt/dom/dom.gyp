# Copyright 2014 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'variables': {
    'cobalt_code': 1,
  },
  'targets': [
    {
      'target_name': 'dom',
      'type': 'static_library',
      'sources': [
        'animation_frame_request_callback_list.cc',
        'animation_frame_request_callback_list.h',
        'array_buffer.cc',
        'array_buffer.h',
        'array_buffer_view.cc',
        'array_buffer_view.h',
        'attr.cc',
        'attr.h',
        'character_data.cc',
        'character_data.h',
        'comment.cc',
        'comment.h',
        'console.cc',
        'console.h',
        'document.cc',
        'document.h',
        'document_builder.cc',
        'document_builder.h',
        'document_type.cc',
        'document_type.h',
        'dom_decoder.cc',
        'dom_decoder.h',
        'dom_exception.cc',
        'dom_exception.h',
        'dom_settings.cc',
        'dom_settings.h',
        'dom_token_list.cc',
        'dom_token_list.h',
        'element.cc',
        'element.h',
        'event.cc',
        'event.h',
        'event_listener.h',
        'event_names.cc',
        'event_names.h',
        'event_queue.cc',
        'event_queue.h',
        'event_target.cc',
        'event_target.h',
        'html_body_element.cc',
        'html_body_element.h',
        'html_collection.cc',
        'html_collection.h',
        'html_div_element.cc',
        'html_div_element.h',
        'html_element.cc',
        'html_element.h',
        'html_element_context.cc',
        'html_element_context.h',
        'html_element_factory.cc',
        'html_element_factory.h',
        'html_head_element.cc',
        'html_head_element.h',
        'html_html_element.cc',
        'html_html_element.h',
        'html_link_element.cc',
        'html_link_element.h',
        'html_media_element.cc',
        'html_media_element.h',
        'html_parser.cc',
        'html_parser.h',
        'html_script_element.cc',
        'html_script_element.h',
        'html_serializer.cc',
        'html_serializer.h',
        'html_span_element.cc',
        'html_span_element.h',
        'html_style_element.cc',
        'html_style_element.h',
        'html_unknown_element.cc',
        'html_unknown_element.h',
        'keyboard_event.cc',
        'keyboard_event.h',
        'location.cc',
        'location.h',
        'media_error.h',
        'media_key_complete_event.cc',
        'media_key_complete_event.h',
        'media_key_error.h',
        'media_key_error_event.cc',
        'media_key_error_event.h',
        'media_key_message_event.cc',
        'media_key_message_event.h',
        'media_key_needed_event.cc',
        'media_key_needed_event.h',
        'media_source.cc',
        'media_source.h',
        'named_node_map.cc',
        'named_node_map.h',
        'navigator.cc',
        'navigator.h',
        'node.cc',
        'node.h',
        'node_children_iterator.h',
        'node_collection.h',
        'node_descendants_iterator.h',
        'node_list.cc',
        'node_list.h',
        'performance.cc',
        'performance.h',
        'performance_timing.cc',
        'performance_timing.h',
        'progress_event.cc',
        'progress_event.h',
        'rule_matching.cc',
        'rule_matching.h',
        'script_event_listener.cc',
        'script_event_listener.h',
        'source_buffer.cc',
        'source_buffer.h',
        'source_buffer_list.cc',
        'source_buffer_list.h',
        'stats.cc',
        'stats.h',
        'text.cc',
        'text.h',
        'time_ranges.cc',
        'time_ranges.h',
        'ui_event.cc',
        'ui_event.h',
        'ui_event_with_key_state.h',
        'uint8_array.cc',
        'uint8_array.h',
        'url.cc',
        'url.h',
        'window.cc',
        'window.h',
        'window_timers.cc',
        'window_timers.h',
      ],
      'dependencies': [
        '<(DEPTH)/cobalt/base/base.gyp:base',
        '<(DEPTH)/cobalt/cssom/cssom.gyp:cssom',
        '<(DEPTH)/cobalt/loader/loader.gyp:loader',
        '<(DEPTH)/cobalt/media/media.gyp:media',
        '<(DEPTH)/googleurl/googleurl.gyp:googleurl',
        '<(DEPTH)/third_party/libxml/libxml.gyp:libxml',
      ],
      'conditions': [
        ['actual_target_arch=="win"', {
          # Disable warning C4702: unreachable code in xtree.
          'msvs_disabled_warnings': [ 4702 ],
        }],
      ],
    },

    {
      'target_name': 'dom_test',
      'type': '<(gtest_target_type)',
      'sources': [
        'comment_test.cc',
        'document_builder_test.cc',
        'document_test.cc',
        'element_test.cc',
        'event_queue_test.cc',
        'event_target_test.cc',
        'event_test.cc',
        'html_parser_test.cc',
        'location_test.cc',
        'node_dispatch_event_test.cc',
        'node_test.cc',
        'performance_test.cc',
        'rule_matching_test.cc',
        'testing/fake_node.h',
        'testing/gtest_workarounds.h',
        'testing/html_collection_testing.h',
        'testing/mock_event_listener.h',
        'testing/stub_css_parser.cc',
        'testing/stub_css_parser.h',
        'testing/stub_script_runner.cc',
        'testing/stub_script_runner.h',
        'testing/switches.cc',
        'testing/switches.h',
        'text_test.cc',
        'time_ranges_test.cc',
        'uint8_array_test.cc',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:run_all_unittests',
        '<(DEPTH)/cobalt/base/base.gyp:base',
        '<(DEPTH)/cobalt/css_parser/css_parser.gyp:css_parser',
        '<(DEPTH)/testing/gmock.gyp:gmock',
        '<(DEPTH)/testing/gtest.gyp:gtest',
        'dom',
      ],
      'actions': [
        {
          'action_name': 'dom_copy_test_data',
          'variables': {
            'input_files': [
              '<(DEPTH)/cobalt/dom/testdata/',
            ],
            'output_dir': 'cobalt/dom/testdata/',
          },
          'includes': [ '../build/copy_data.gypi' ],
        },
      ],
    },

    {
      'target_name': 'dom_test_deploy',
      'type': 'none',
      'dependencies': [
        'dom_test',
      ],
      'variables': {
        'executable_name': 'dom_test',
      },
      'includes': [ '../build/deploy.gypi' ],
    },
  ],
}
