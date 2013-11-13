<?php
namespace ECK\UI\Web;
use ECK\Core\IUserInterface;
use ECK\Core\System;
use ECK\Core\Properties;

class WebInterface implements IUserInterface{
  
  public function parseInitialUserInput( System $system, $user_input){
    $operation = array_shift($user_input);
    $object_type = array_shift($user_input);
    
    $system->setOperation($operation);
    
    //the rest of pieces of user input are objects that should be loade in the
    //system
    foreach($user_input as $ui){
      $pieces = explode(":", $ui);
      $system->setObject($pieces[0], $pieces[1]);
    }
    
    $system->setMainObjectType($object_type);

    /*$id = "";

    $counter = 0;
    foreach($args as $arg){
      if(is_string($arg)){
        if($counter > 0){
          $id .= "|";
        }
        $id .= $arg;
        $counter++;
      }else if(is_object($arg)){
        $system->addUserInput($arg);
      }
    }
    
    if(!empty($id)){
      $system->setObjectId($id);
    }*/
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
      $object = array();
      foreach($properties as $property){
        $object[$property->getName()] = 
          ($property->getDefaultValue())?$property->getDefaultValue():'';
      }

      $fb = new \ECK\UI\Widgets\FormBuilder($object);
      foreach($properties as $property){
        $fb->addWidget($property->getName(), $property->getWidget());
      }

      $form = drupal_get_form('eck_master_form', $fb, $system);
      return $form;
    }else{
      $form = drupal_get_form('eck_master_confirm_form', $system);
      return $form;
    }
  }
  
  
  public function outputToUser(System $system, $output) {
    if(is_string($output)){
      drupal_set_message($output);
      return "";
    }else if(is_array($output)){
      return $this->displayObjects($output);
    }else if(is_object($output)){
      return $this->displayObject($output);
    }
  }
  
  private function displayObject($object){
    //lets cheat a little bit, and lets get our formatters code in here for entities only at the
    //moment
    if(get_class($object) == "ECK\Core\EEntity"){
      $entity_type = $object->getEntityType();
      $properties = $entity_type->getProperties();
      $vb = new \ECK\UI\Formatters\ViewBuilder($object);
      foreach($properties as $property){
        /*$name = $property->getName();
        $value = $object->{$name};
        $html .= "<p>{$value}</p>";*/
        $vb->addFormatter($property->getName(), $property->getFormatter());
        $html = $vb->build();
      }
      return $html;
    }else{
      return "<p>{$object->getName()}</p>";
    }
  }
  
  private function displayObjects($objects){
    
    global $eck_system;
    $object_type = $eck_system->getMainObjectType();
    
    $path = current_path();
    
    $cluster = \ECK\UI\Web\MenuCluster::getClusterFromUrl($object_type, $path);
    
    //this are the names of the operations 
    $operations = $cluster->getOperations();
 
    $rows = array();
    $header = array(t('ID'), array('data' => t('Operations'), 'colspan' => '1'));

    foreach ($objects as $object) {
      $row = array();
      $row[0] = $this->displayObject($object);
      $row[1] = "";
      $counter = 0;
      foreach($operations as $op){
        
        $instance_path = $cluster->getOperationInstancePath($object, $op);
        if($instance_path){
          if($counter > 0){
            $row[1].= " | ";
          }
          $row[1] .= "<a href='/{$instance_path}'>{$cluster->getOperationAlias($op)}</a>";
          $counter++;
        }
      }
      $rows[] = $row;
    }

    return array('#theme' => 'table', '#header' => $header, '#rows' => $rows);
  }
 
}
