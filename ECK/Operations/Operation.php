<?php

namespace ECK\Operations;
abstract class Operation {
  
  protected abstract function getOperation();
  
  /**
   *This method utilizes a system defined by eck to get object requirements
   * for a given operation, for more info look at hook_eck_operation_info
   * @return \ECK\Core\Properties 
   */
  public function getRequirements($user_input = NULL){
    global $eck_system;
    
    $object_type = $eck_system->getMainObjectType();
    
    $op_info = eck_get_operation_info($this->getOperation());
    $obj_info = eck_get_object_type_info($object_type);
    $obj_class = $obj_info['class'];
    $object = $eck_system->getMainObject();
   
    $requirements = NULL;
    if(array_key_exists($object_type, $op_info) && 
            array_key_exists('requirements', $op_info[$object_type])){
      $requirements = $op_info[$object_type]['requirements'];
    }
    
    if(is_array($requirements)){
      $properties = new \ECK\Core\Properties();
      foreach($requirements as $r){
        $properties->addProperty($obj_class::getMetaProperty($r, $object));
      }
      return $properties;
   }else if(is_string($requirements)){
     $properties = call_user_func(array($obj_class, $requirements));
     return $properties;
   }else{
     return NULL;
   }
  }
  
  //I guess this is bad form too, since the class is not really using this methods
  //but I hate to create an interface that I believe will not be used by other
  //class but this one
  public abstract function operate($user_input = NULL);
}

?>
