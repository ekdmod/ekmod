/*
field_ui_inactive_message.7 field_ui.admin.inc field_ui_inactive_message($entity_type, $bundle) 
8 field_ui.admin.inc field_ui_inactive_message($entity_type, $bundle) 

Displays a message listing the inactive fields of a given bundle.

2 calls to field_ui_inactive_message()
field_ui_display_overview_form in modules/field_ui/field_ui.admin.incForm constructor for the field display settings for a given view mode.field_ui_field_overview_form in modules/field_ui/field_ui.admin.incForm constructor for the 'Manage fields' form of a bundle.
File

modules/field_ui/field_ui.admin.inc, line 56
Administrative interface for custom field type creation.

Code
*/
<?php 

function field_ui_inactive_message($entity_type, $bundle) {
  $inactive_instances = field_ui_inactive_instances($entity_type, $bundle);
  if (!empty($inactive_instances)) {
    $field_types = field_info_field_types();
    $widget_types = field_info_widget_types();

    foreach ($inactive_instances as $field_name => $instance) {
      $list[] = t('%field (@field_name) field requires the %widget_type widget provided by %widget_module module', array(
        '%field' => $instance['label'],
        '@field_name' => $instance['field_name'],
        '%widget_type' => isset($widget_types[$instance['widget']['type']]) ? $widget_types[$instance['widget']['type']]['label'] : $instance['widget']['type'],
        '%widget_module' => $instance['widget']['module'],
      ));
    }
    drupal_set_message(t('Inactive fields are not shown unless their providing modules are enabled. The following fields are not enabled: !list', array('!list' => theme('item_list', array('items' => $list)))), 'error');
  }
} 

?>