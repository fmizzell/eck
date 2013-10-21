<?php

/**
 * Load callback for %eckentity
 */
function eckentity_load($id, $type) {
  return entity_load_single($type, $id);
}

function entity_type_load($entity_type_name){
  return EntityType::loadByName($entity_type_name);
}

function bundle_load($entity_type_name, $bundle_name){
  return Bundle::loadByMachineName("{$entity_type_name}_{$bundle_name}");
}
