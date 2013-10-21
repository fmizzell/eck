<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class Decimal implements PropertyType{
  
  public static function schema() {
    $schema = array(
      'description' => 'Decimal/Float/Double',
      'type' => 'float',
      'not null' => TRUE,
      'default' => 0,
      //relevant only to int and float
      'unsigned' => FALSE
    );
    
    return $schema;
  }
  
  public static function validate($value){
    $number = filter_var($value, FILTER_VALIDATE_FLOAT);
    return ($number !== FALSE);
  }
}
