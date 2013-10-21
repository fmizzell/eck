<?php
namespace ECK\PropertyTypes;
// Generic types supported by schema api
// 'varchar', 'char', 'int', 'serial', 'float', 'numeric', 'text', 'blob' or 'datetime'
interface IPropertyType{
  public static function schema();
  public static function validate($value);
}
