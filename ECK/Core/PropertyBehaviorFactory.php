<?php

namespace ECK\Core;
class PropertyWidgetFactory {
  public static function get($name){
    global $eck_system;
    $property = $eck_system->getFromContext('property');
    return PropertyWidget::load($property, $name);
  }
}

