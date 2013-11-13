<?php
namespace ECK\Core;
use ECK\UI\Formatters\Formatter;
class PropertyFormatter extends Formatter{
  private $is_new;
  
  private $type;
  private $formatter;
  
  public function __construct(\ECK\Core\Property $property, $type){
    //lets get the widget types and instantiate a widget of the appropriate type
    $this->type = $type;
    $info = eck_get_formatter_info($type);
    $this->formatter = new $info['class']($property);
    $this->is_new = TRUE;
    
    parent::__construct($property);
  }
  
  public function getType(){
    return $this->type;
  }
  
  public function getSettingDefaultValue($setting) {
    return $this->formatter->getSettingDefaultValue($setting);
  }
  
  public function getSettings() {
    return $this->formatter->getSettings();
  }
  
   public static function getMetaProperty($property_name, PropertyFormatter $formatter = NULL){
    
    if ($property_name == "type"){
      $p = new Property('type', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Type";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($formatter){
        $p->setDefaultValue($formatter->getType());
      }
      
      return $p;
    }
    
    return NULL;
  }

  public function display($value) {
    return $this->formatter->display($value);
  }
  
  public function save(){
    $this->property->setFormatter($this);
    $this->property->save();
  }
  
  public function serialize(){
    $p = array();
    $p['type'] = $this->type;
    
    return drupal_json_encode($p);
  }
  
  public static function deserialize(\ECK\Core\EntityProperty $property, $string){
    $p = drupal_json_decode($string);
    return new PropertyFormatter($property, $p['type']);
  }
  
  public static function load($property, $name){
    return $property->getFormatter();
  }
}
