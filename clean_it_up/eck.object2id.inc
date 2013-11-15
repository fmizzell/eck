<?php

/**
 * Given any eck object type, and the actual object, generate a string id for the
 * object
 * 
 * @param type $object_type
 * @param type $object
 * @return type 
 */
function eck_object2id($object_type, $object){
  if($object_type == "entity_type"){
    return $object->name;
  }else if($object_type == "bundle"){
    return "{$object->entity_type}|{$object->name}";
  }else if($object_type == "entity"){
    return "{$object->entityType()}|{$object->type}|{$object->id}";
  }
}

function eck_id2object($object_type, $object_id){
  if($object_type == 'entity_type'){
    return array_shift(EntityType::load($object_id));
  }
}
