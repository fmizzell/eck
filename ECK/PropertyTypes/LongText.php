<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class LongText implements PropertyType{
  public static function schema(){
    $schema = array(
      'description' => 'Long Text (longer than 8000 characters)',
      'type' => 'text',
      'length' => 255,
      'not null' => TRUE,
      //relevant only to: varchar, char, and text
      'length' => 255,
    );
    
    return $schema;
  }

  public static function validate($value) {
    //Pretty much anything can be store in a text field
    //even an object?
    //@TODO check what happens when trying to save an obejct to a varchar field
    //in the db
    
    return TRUE;
  }
}
