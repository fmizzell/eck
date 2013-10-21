<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\PropertyType;

class FixedSizeText implements PropertyType{
  public static function schema(){
    $schema = array(
      'description' => 'All of the  values of this property are text of the same size',
      'type' => 'char',
      'not null' => TRUE,
      'default' => '',
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
