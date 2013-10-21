<?php
namespace ECK\Core;
class EntityTypeFactory {
  public static function get($name){
    return EntityType::load($name);
  }
}

?>
