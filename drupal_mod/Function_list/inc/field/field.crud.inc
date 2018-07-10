/*
field_read_instances.7 field.crud.inc field_read_instances($params = array(), $include_additional = array()) 
8 field.crud.inc field_read_instances($params = array(), $include_additional = array()) 

Reads in field instances that match an array of conditions.

Parameters
$param: An array of properties to use in selecting a field instance. Valid keys include any column of the field_config_instance table. If NULL, all instances will be returned.

$include_additional: The default behavior of this function is to not return field instances that have been marked deleted, or whose field is inactive. Setting $include_additional['include_inactive'] or $include_additional['include_deleted'] to TRUE will override this behavior.

Return value
An array of instances matching the arguments.

Related topics
Field CRUD API
Create, update, and delete Field API fields, bundles, and instances.
14 calls to field_read_instances()
FieldBulkDeleteTestCase::testDeleteFieldInstance in modules/field/tests/field.testVerify that deleting an instance leaves the field data items in the database and that the appropriate Field API functions can operate on the deleted data and instance.FieldBulkDeleteTestCase::testPurgeInstance in modules/field/tests/field.testVerify that field data items and instances are purged when an instance is deleted.FieldInfo::getBundleInstances in modules/field/field.info.class.incRetrieves the instances for a bundle.FieldInfo::getInstances in modules/field/field.info.class.incRetrieves all active, non-deleted instances definitions.field_attach_delete_bundle in modules/field/field.attach.incNotify field.module the a bundle was deleted.... See full list

File

modules/field/field.crud.inc, line 698
Field CRUD API, handling field and field instance creation and deletion.

Code
*/
<?php 
function field_read_instances($params = array(), $include_additional = array()) {
  $include_inactive = isset($include_additional['include_inactive']) && $include_additional['include_inactive'];
  $include_deleted = isset($include_additional['include_deleted']) && $include_additional['include_deleted'];

  $query = db_select('field_config_instance', 'fci', array('fetch' => PDO::FETCH_ASSOC));
  $query->join('field_config', 'fc', 'fc.id = fci.field_id');
  $query->fields('fci');

  // Turn the conditions into a query.
  foreach ($params as $key => $value) {
    $query->condition('fci.' . $key, $value);
  }
  if (!$include_inactive) {
    $query->condition('fc.active', 1)->condition('fc.storage_active', 1);
  }
  if (!$include_deleted) {
    $query->condition('fc.deleted', 0);
    $query->condition('fci.deleted', 0);
  }

  $instances = array();
  $results = $query->execute();

  foreach ($results as $record) {
    // Filter out instances on unknown entity types (for instance because the
    // module exposing them was disabled).
    $entity_info = entity_get_info($record['entity_type']);
    if ($include_inactive || $entity_info) {
      $instance = unserialize($record['data']);
      $instance['id'] = $record['id'];
      $instance['field_id'] = $record['field_id'];
      $instance['field_name'] = $record['field_name'];
      $instance['entity_type'] = $record['entity_type'];
      $instance['bundle'] = $record['bundle'];
      $instance['deleted'] = $record['deleted'];

      module_invoke_all('field_read_instance', $instance);
      $instances[] = $instance;
    }
  }
  return $instances;
}
function ekmod_read_instances($params = array(), $include_additional = array()) {
  $include_inactive = isset($include_additional['include_inactive']) && $include_additional['include_inactive'];
  $include_deleted = isset($include_additional['include_deleted']) && $include_additional['include_deleted'];

  $query = db_select('field_config_instance', 'fci', array('fetch' => PDO::FETCH_ASSOC));
  $query->join('field_config', 'fc', 'fc.id = fci.field_id');
  $query->fields('fci');

  // Turn the conditions into a query.
  foreach ($params as $key => $value) {
    $query->condition('fci.' . $key, $value);
  }
  if (!$include_inactive) {
    $query->condition('fc.active', 1)->condition('fc.storage_active', 1);
  }
  if (!$include_deleted) {
    $query->condition('fc.deleted', 0);
    $query->condition('fci.deleted', 0);
  }

  $instances = array();
  $results = $query->execute();

  foreach ($results as $record) {
    // Filter out instances on unknown entity types (for instance because the
    // module exposing them was disabled).
    $entity_info = entity_get_info($record['entity_type']);
    if ($include_inactive || $entity_info) {
      $instance = unserialize($record['data']);
      $instance['id'] = $record['id'];
      $instance['field_id'] = $record['field_id'];
      $instance['field_name'] = $record['field_name'];
      $instance['entity_type'] = $record['entity_type'];
      $instance['bundle'] = $record['bundle'];
      $instance['deleted'] = $record['deleted'];

 //   module_invoke_all('field_read_instance', $instance);
      $instances[] = $instance;
    }
  }
  return $instances;
}
?>