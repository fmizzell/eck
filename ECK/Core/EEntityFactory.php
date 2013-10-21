<?php

namespace ECK\Core;
class EEntityFactory {
  public static function get($id){
    global $eck_system;
    $entity_type = $eck_system->getFromContext('entity_type');
    return EEntity::load($entity_type, $id);
  }
}


