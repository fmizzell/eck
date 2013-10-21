<?php

namespace ECK\UI;

class PropertyPage{
  public static function listing($entity_type){
    $current_path = current_path();

    //lets get the default properties
    //$default_properties = eck_get_default_properties();
    //let get the actual properties of the entity type
    $entity_type_properties = $entity_type->getProperties();

    //lets arrenge the table with the appropiate action links

    //This is the table where all properties are shown.
    $header = array(
      'name' => t('Name'),
      'label' => t('Label'),
      'type' => t('Type'),
      'behavior' => t('Behaviour'),
      'operations' => t('Operations')
    );

    $options = array();
    /*//lets process default properties first
    foreach($default_properties as $machine_name => $info){
      $default_property_row = array();
      $info['machine_name'] = $machine_name;
      foreach($header as $key => $label){
        if($key == 'name'){
          $key = "label";
        }  
        if($key != 'operations'){
          $default_property_row[] = array_key_exists($key, $info)?$info[$key]:"";
        }
      }

      if(array_key_exists($machine_name, $entity_type_properties)){
        $default_property_row[] = l('Deactivate', $current_path."/".$machine_name."/deactivate");
        unset($entity_type_properties[$machine_name]);
      }else{
         $default_property_row[] = l('Activate', $current_path."/".$machine_name."/activate") ;
      }

      $options[] = $default_property_row;
    }*/

    //ok, now lets do our custom properties
    //lets process default properties first
    foreach($entity_type_properties as $property){
      $default_property_row = array();
      $info['name'] = $property->getName();
      foreach($header as $key => $label){
        if($key != 'operations'){
          $default_property_row[] = array_key_exists($key, $info)?$info[$key]:"";
        }
      }

      $operations = "";
      foreach(array('edit' => "Edit", 'delete' => "Delete", "behavior" => "behavior") as $op => $label){
        if($op == 'behavior' && !empty($info['behavior'])){
          $label = "Edit ".$label;
        }else if($op == 'behavior' && empty($info['behavior'])){
          $label = "Add ".$label;
        }

        $operations .= l($label, $current_path."/".$property->getName()."/{$op}")."<br>";
      }

      $default_property_row[] = $operations;

      $options[] = $default_property_row;
    }

    $build['properties_table'] = 
    array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $options,
      '#empty' => t('No other properties for this entity type.'),
    );

    return $build;
  }
  
  public static function add($entity_type){
    module_load_include("php", 'eck', 'core/property/forms');
    $form = drupal_get_form('eck__property__edit_form', $entity_type);
    return $form;
  }
  
  public static function edit($entity_type){
    return "Hello"; 
  }
  
  public static function delete($entity_type){
    return "Hello";
  }
  
  public static function addBehavior($entity_type){
    return "Hello";
  }
  
  public static function removeBehavior($entity_type){
    return "Hello";
  }
}
