/*
field_ui_inactive_instances.7 field_ui.module field_ui_inactive_instances($entity_type, $bundle_name = NULL) 
8 field_ui.module field_ui_inactive_instances($entity_type, $bundle_name = NULL) 

Identifies inactive fields within a bundle.

1 call to field_ui_inactive_instances()
field_ui_inactive_message in modules/field_ui/field_ui.admin.incDisplays a message listing the inactive fields of a given bundle.
File

modules/field_ui/field_ui.module, line 334
Allows administrators to attach custom fields to fieldable types.

Code
*/
<?php 
function field_ui_inactive_instances($entity_type, $bundle_name = NULL) {
  $params = array('entity_type' => $entity_type);

  if (empty($bundle_name)) {
    $active = field_info_instances($entity_type);
    $inactive = array();
  }
  else {
    // Restrict to the specified bundle. For consistency with the case where
    // $bundle_name is NULL, the $active and  $inactive arrays are keyed by
    // bundle name first.
    $params['bundle'] = $bundle_name;
    $active = array($bundle_name => field_info_instances($entity_type, $bundle_name));
    $inactive = array($bundle_name => array());
  }

  // Iterate on existing definitions, and spot those that do not appear in the
  // $active list collected earlier.
  $all_instances = field_read_instances($params, array('include_inactive' => TRUE));
  foreach ($all_instances as $instance) {
    if (!isset($active[$instance['bundle']][$instance['field_name']])) {
      $inactive[$instance['bundle']][$instance['field_name']] = $instance;
    }
  }

  if (!empty($bundle_name)) {
    return $inactive[$bundle_name];
  }
  return $inactive;
}
?>