<?php
namespace ECK\PropertyTypes;
use ECK\PropertyTypes\Integer;

class PositiveInteger extends Integer{
  
  public static function schema() {
    $schema = parent::schema();
    
    $schema['description'] = "Positive Integer";
    $schema['unsigned'] = TRUE;
    
    return $schema;
  }
  
  public static function validate($value){
    if(parent::validate($value)){
      
      //lets cast it in case it is string
      $int = intval($value);
      if($int >= 0){
        return TRUE;
      }
    }
    
    return FALSE;
  }
}
