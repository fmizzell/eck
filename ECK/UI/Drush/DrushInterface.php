<?php
namespace ECK\UI\Drush;
use ECK\Core\IUserInterface;
use ECK\Core\System;
use ECK\Core\Properties;

class DrushInterface implements IUserInterface{
  
  public function parseInitialUserInput( System $system, $user_input){
    $object_type = array_shift($user_input);
    $operation = array_shift($user_input);
    $id = array_shift($user_input);
    
    $system->setOperation($operation);
    
    $system->setMainObjectType($object_type);
  
    //we are going to parse with the dependencies and set up the objects appropiately
    if($id){
      $dependencies = $this->getDependencies($object_type);
      $pieces = explode("-", $id);
      foreach($pieces as $piece){
        $objtype = array_pop($dependencies);
        $system->setObject($objtype, $piece);
      }
    }
  }
  
  public function getDependencies($object_type, $dependencies = array()){
    $dependencies[] = $object_type;
    $info = eck_get_object_type_info($object_type);
    if(array_key_exists('dependencies', $info)){
      $dependencies = $this->getDependencies($info['dependencies'][0], $dependencies);
    }
    return $dependencies;
  }
    
  /**
   * If we get a list of requirements, those are needed from the user
   * If no requirements are given, we just need confirmation
   * @param System $system
   * @param Properties $requirements 
   */
  public function getRequirementsFromUser(System $system, Properties $requirements){
    $properties = $requirements->getProperties();
    if(!empty($properties)){
      foreach($requirements->getProperties() as $r){
        $input = drush_prompt($r->getLabel(), $r->getDefaultValue());
        $system->addUserInput($input, $r->getName());
      }
      $system->performOperation();
    }else{
      $system->performOperation();
    }
  }
  
  
  public function outputToUser(System $system, $output) {
    if(is_string($output)){
      drush_log($output, 'success');
    }else if(is_array($output)){
      foreach($output as $object){
        drush_log($object->getName(), 'success');
      }
    }else if(is_object($output)){
      drush_print_r($output);
    }
  }
}

