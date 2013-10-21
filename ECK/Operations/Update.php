<?php

namespace ECK\Operations;
use ECK\Operations\Operation;
class Update extends Operation {
  
  public function operate($user_input = NULL){
    global $eck_system;
    $object= $eck_system->getMainObject();
    
    foreach($user_input as $property => $value){
      $p = eck_to_camel_case($property, TRUE);
      $method = "set{$p}";
      $object->{$method}($value);
    }
    $object->save();
  }

  protected function getOperation() {
    return 'update';
  }
}