<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class DateTime implements PropertyType{
  
  public static function schema() {
    $schema = array(
      'description' => 'Date/Time',
      'type' => 'datetime',
      'not null' => TRUE,
      'default' => 0,
    );
    
    return $schema;
  }
  
  public static function validate($value){
    $number = filter_var($value, FILTER_VALIDATE_FLOAT);
    return ($number !== FALSE);
  }
}
