<?php
namespace ECK\Operations;
use ECK\Operations\Operation;
class Delete extends Operation {
  
  public function operate($user_input = NULL){
    global $eck_system;
    $object = $eck_system->getMainObject();
    $object->delete();
  }

  public function getOperation() {
    return 'delete';
  }
}
