<?php
namespace ECK\Core;
class PropertyBehavior{
  private $is_new;
  
  private $property;
  private $type;
  private $info;
  
  public function __construct(\ECK\Core\Property $property, $type){
    //lets get the widget types and instantiate a widget of the appropriate type
    $this->property = $property;
    $this->type = $type;
    $this->info = eck_get_behavior_info($type);
    $this->is_new = TRUE;
  }
  
  public function getType(){
    return $this->type;
  }
  
  /*public function getSettingDefaultValue($setting) {
    return $this->widget->getSettingDefaultValue($setting);
  }
  
  public function getSettings() {
    return $this->widget->getSettings();
  }*/
  
   public static function getMetaProperty($property_name, $type = NULL){
    
    if ($property_name == "type"){
      $p = new Property('type', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Type";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($type){
        $p->setDefaultValue($type);
      }
      
      return $p;
    }
    
    return NULL;
  }

  public function behave($method, $args) {
    $class = $this->info['class'];
    return call_user_func_array(array($class, $method), $args);
  }
  
  public function save(){
    $this->property->setBehavior($this);
    $this->property->save();
  }
  
  public function serialize(){
    $p = array();
    $p['type'] = $this->type;
    
    return drupal_json_encode($p);
  }
  
  public static function deserialize(\ECK\Core\EntityProperty $property, $string){
    $p = drupal_json_decode($string);
    return new PropertyBehavior($property, $p['type']);
  }
  
  public static function load($property, $name){
    return $property->getBehavior();
  }
}
