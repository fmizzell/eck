<?php
namespace ECK\Core;
use ECK\UI\Widgets\Widget;
class PropertyWidget extends Widget{
  private $is_new;
  
  private $type;
  private $widget;
  
  public function __construct(\ECK\Core\Property $property, $type){
    //lets get the widget types and instantiate a widget of the appropriate type
    $this->type = $type;
    $info = eck_get_widget_info($type);
    $this->widget = new $info['class']($property);
    $this->is_new = TRUE;
    
    parent::__construct($property);
  }
  
  public function getType(){
    return $this->type;
  }
  
  public function getSettingDefaultValue($setting) {
    return $this->widget->getSettingDefaultValue($setting);
  }
  
  public function getSettings() {
    return $this->widget->getSettings();
  }
  
   public static function getMetaProperty($property_name, PropertyWidget $widget = NULL){
    
    if ($property_name == "type"){
      $p = new Property('type', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Type";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($widget){
        $p->setDefaultValue($widget->getType());
      }
      
      return $p;
    }
    
    return NULL;
  }

  public function display($value) {
    return $this->widget->display($value);
  }
  
  public function save(){
    $this->property->setWidget($this);
    $this->property->save();
  }
  
  public function serialize(){
    $p = array();
    $p['type'] = $this->type;
    
    return drupal_json_encode($p);
  }
  
  public static function deserialize(\ECK\Core\EntityProperty $property, $string){
    $p = drupal_json_decode($string);
    return new PropertyWidget($property, $p['type']);
  }
  
  public static function load($property, $name){
    $property->getWidget();
  }
}
