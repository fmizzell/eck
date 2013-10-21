<?php
namespace ECK\UI\Widgets;
use Exception;
//This class takes and object and some widgets, to build a form
//to get input for that object

class FormBuilder{
  
  private $widgets;
  private $object;
  
  public function __construct($object){
    //the object can be an object or an array
    if(is_object($object)){
      $this->object = $object;
    }else if(is_array($object)){
      $this->object = (object)$object;
    }else{
      throw new Exception("Object should be an array or an object");
    }
  
    $this->widgets = array();
  }
  
  public function addWidget($property_name, $widget){
    if($widget){
      $this->widgets[$property_name] = $widget;
    }
  }
  
  private function getValue($property_name){
    $method_name = "get".ucfirst($property_name);
    if(isset($this->object->{$property_name})){
      return $this->object->{$property_name};
    }else if(method_exists($this->object, $method_name)){
      return $this->object->{$method_name}();
    }

    throw new Exception("property {$property_name} does not exist");
  }
  
  public function build($form){
    foreach($this->widgets as $property_name => $widget){
      $value = $this->getValue($property_name);
      $form[$property_name] = $widget->display($value);
    }
    
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
    
    return $form;
  }
  
  /**
   *
   * @param (array) $values and array of values for each property, so the object can
   * be updated 
   */
  public function update($values){
    foreach(array_keys($this->widgets) as $property_name){
      if(array_key_exists($property_name, $values)){
        $this->setValue($property_name, $values[$property_name]);
      }
    }
  }
  
  private function setValue($property_name, $value){
    $method_name = "set".ucfirst($property_name);  

    if(isset($this->object->{$property_name})){
      $this->object->{$property_name} = $value;
    }else if(method_exists($this->object, $method_name)){
      $this->object->{$method_name}($value);
    }else{
      throw new Exception("no way to set the property {$property_name}");
    }
  }
  
  public function getObject(){
    return $this->object;
  }
  
}