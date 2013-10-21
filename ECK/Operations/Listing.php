<?php
namespace ECK\Operations;
use ECK\Operations\Operation;
class Listing extends Operation{

  protected function getOperation() {
    return 'listing';
  }

  public function operate($user_input = NULL) {
    global $eck_system;
    $obj_info = eck_get_object_type_info($eck_system->getMainObjectType());
    $class = $obj_info['class'];
    $dependencies = array();
    if(array_key_exists('dependencies', $obj_info)){
      foreach($obj_info['dependencies'] as $d){
        $dependencies[] = $eck_system->getFromContext($d);
      }
    }
    $list = call_user_func_array(array($class, "loadAll"), 
            array_merge($dependencies, $user_input));
    return $list;
  }
}
