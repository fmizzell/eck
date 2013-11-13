<?php
namespace ECK\UI\Formatters;
use Exception;
//This class takes and object and some widgets, to build a form
//to get input for that object

class ViewBuilder{
  
  private $formatters;
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
  
    $this->formatters = array();
  }
  
  public function addFormatter($property_name, $formatter){
    if($formatter){
      $this->formatters[$property_name] = $formatter;
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
  
  public function build(){
    $view = '';
    foreach($this->formatters as $property_name => $formatter){
      $value = $this->getValue($property_name);
      $view .= $formatter->display($value);
    }
    
    return $view;
  }
}