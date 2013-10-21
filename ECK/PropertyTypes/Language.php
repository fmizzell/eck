<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\Text;

class Language extends Text{
  
  public static function schema(){
    $schema = parent::schema();
    
    $schema['default'] = LANGUAGE_NONE;
    $schema['description'] = "Language";
    $schema['length'] = 12;
    
    return $schema;
  }
  
  public static function validate($value){
    //make sure it is text
    if(parent::validate($value)){
      //@TODO check with the language people, what kind of validation can we
      //do here
      return TRUE;
    }
    
    return FALSE;
  }
}
