<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\FixedSizeText;

class UUID extends FixedSizeText{
  
  public static function schemaConditions(){
    return array('locked' => 'length');
  }
  
  public static function schema() {
    $schema = parent::schema();
    $schema ['description'] = 'Universally Unique Identifier';
    $schema['length'] = 16;
    
    return $schema;
  }
  
  public static function validate($value){
    //@TODO UUID module has a function ... steal :)
    return TRUE;
  }
}
