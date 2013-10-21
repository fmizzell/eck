<?php

namespace ECK\Core;
use ECK\Core\Property;

class Properties {
  private $properties;
  
  public function __construct(){
    $this->properties = array();
  }
  
  public function addProperty(Property $property){
    $this->properties[] = $property;
  }
  
  public function getProperties(){
    return $this->properties;
  }
}
