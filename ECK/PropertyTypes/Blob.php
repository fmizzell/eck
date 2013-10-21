<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class Blob implements PropertyType{
  
  public static function schema() {
    $schema = array(
      'description' => 'Decimal/Float/Double',
      'type' => 'blob',
      'size' => 'normal',
      'not null' => TRUE,
    );
    
    return $schema;
  }
  
  public static function validate($value){
    $number = filter_var($value, FILTER_VALIDATE_FLOAT);
    return ($number !== FALSE);
  }
}
