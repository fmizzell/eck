<?php
namespace ECK\Core;
class EntityPropertyFactory {
  public static function get($name){
    global $eck_system;
    $entity_type = $eck_system->getFromContext('entity_type');
    return EntityProperty::load($entity_type, $name);
  }
}

