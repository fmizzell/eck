<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class Integer implements PropertyType{
  
  public static function schema() {
    $schema = array(
      'description' => "Integer",
      'type' => 'int',
      'size' => 'normal',
      'not null' => TRUE,
      'default' => 0,
      //relevant only to int and float
      'unsigned' => FALSE
    );
    
    return $schema;
  }
  
  public static function validate($value){
    $number = filter_var($value, FILTER_VALIDATE_INT);
    return ($number !== FALSE);
  }
}
