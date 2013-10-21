<?php
namespace ECK\Core;

class KeyValues {
  
  private $values;
  
  public function __construct(){
    $this->$values = array();
  }
  
  public function addValue($key, $value){
    $this->values[$key] = $value;
  }
  
  public function getValue($key){
    if(array_key_exists($key, $this->values)){
      return $this->values[$key];
    }
    
    return NULL;
  }
  
  public function getKeys(){
    return array_keys($this->values);
  }
  
  public function getKeyValues(){
    $this->values;
  }
  
  
  
}

