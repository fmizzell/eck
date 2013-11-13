<?php
namespace ECK\Operations;
use ECK\Operations\Operation;
class Read extends Operation {
  
  public function operate($user_input = NULL){
    global $eck_system;
    $object= $eck_system->getMainObject();
    return $object;
  }

  protected function getOperation() {
    return 'read';
  }
}
