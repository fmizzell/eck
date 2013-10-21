<?php
namespace ECK\Operations;
use ECK\Operations\Operation;
class Create extends Operation {
  
  //this function connects this operations with the requirements system
  //defined by eck
  //for more info look at hook_eck_operation_info();
  protected function getOperation(){
    return "create";
  }
  
  public function operate($user_input = NULL){
    global $eck_system;
    $obj_type = $eck_system->getMainObjectType();
    $obj_info = eck_get_object_type_info($obj_type);
    $obj_class = $obj_info['class'];
    
    //lets cheat a little just to get this done
    if($obj_type == 'entity'){
      $entity_type = $eck_system->getFromContext('entity_type');
      $values = $user_input;
      $user_input = array();
      $user_input['values'] = $values;
      $user_input['entity_type'] = $entity_type->getName();
    }
    
    $reflection = new \ReflectionClass($obj_class);
    //lets check to see if our object has any dependencies
    $dependencies = array();
    if($obj_type != 'entity'){
      if(array_key_exists('dependencies', $obj_info)){
        foreach($obj_info['dependencies'] as $d){
          $dependencies[] = $eck_system->getFromContext($d);
        }
      }
    }
    $object = $reflection->newInstanceArgs(array_merge($dependencies, $user_input));
    $object->save();
  }
}

