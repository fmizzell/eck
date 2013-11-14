<?php
/**
 * @file
 * All the menus, pages, and functionality related to administering entities.
 */

//a few helper function to get data our of the info array
function get_bundle_admin_info($entity_type, $bundle){
  $info = entity_get_info();
  
  return $info[$entity_type]['bundles'][$bundle]['admin'];
}
function get_bundle_crud_info($entity_type_name, $bundle_name){
  $info = entity_get_info();
  return $info[$entity_type_name]['bundles'][$bundle_name]['crud'];
}


/**
 * Get the entities view
 *
 * @param $entity_type
 *  (String) entity type
 * @param $id
 *  (mixed) The entity id or the entity object itself
 *  Normally you wouldn't need to call this function if you already have the
 *  loaded entity but there are some workflows where an object is passed.
 *  So this function handles that case as well.
 */
function eck__entity__build($entity_type, $bundle, $entity) {
  
  if (!$entity) {
    drupal_not_found();
    exit();
  }
  else {
    if ($entity->type == $bundle->name) {
      return $entity->view();
    }
    else {
      drupal_not_found();
      exit();
    }
  }
}

