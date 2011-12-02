<?php

function eck_set_properties_schema(&$schema, $entity_type){
  $properties = $entity_type->properties;
  
  foreach($properties as $name => $info){
      $type = $info['type'];
      $schema['fields'][$name] = eck_property_type_schema($type);
  }
}

function eck_property_types(){
  //Alll types: serial, int, float, numeric, varchar, char, text, blob, datetime
  return array('text' => 'Text', 'integer' => "Interger");
}

function eck_property_type_schema($type){
  $schema = array();
  
  switch ($type) {
    case 'text':
      $schema = array(
        'description' => 'Text',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    case 'decimal':
      $schema = array(
        'description' => 'Decimal',
        'type' => 'float',
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'integer':
    //case 'date':
    //default:
    // Integer, date, and entities are all of type 'int'.
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'default' => 0,
      );
      break;
    
    default:
      $schema = NULL;
      break;
  }
  return $schema;
}


/**
 * Generates schema definition for additional ECK properties.
 *
 * @param $property_name
 *   Property to generate schema definition for.
 * @param $entity_type
 *   (Object) An object with and entity types info, as returned by entity_type__load 
 * This is only use to add stuff to the descriptions of the property, do we really need this?
 * Well, if people is already using the module it will be a pain in the butt to syncronize their schemas to use
 * indexes, and so on without the specifics of the entity type, so for the sake of moving forward to more important
 * things, lets leave the $entity_type as a parameter for this function.
 */
/*function eck__property__schema(&$schema, $entity_type, $property_name) {
}*/

/*function eck__property__form($entity_type){
  $form = array();
  // Enable/disable properties.
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties'),
    '#description' => t('Note: unchecking will delete the property and all data stored within!'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  
  $properties = hook_eck_property_info();
 
  foreach ($properties as $property => $info) {
    
      $form['properties'][$property] = array(
        '#type' => 'checkbox',
        '#title' => $info['label'],
        '#default_value' => 
        isset($entity_type->properties) &&
        array_key_exists($property, $entity_type->properties) ? 1 : 0
      );
  }
  
  return $form;
}*/

function  eck__propeties__form_default_properties($form, &$state, $entity_type){
   //DEFAULT PROPERTIES
  //Now we want to display the default properties
  $default_properties = eck_get_default_properties();
  $options = array();
  foreach($default_properties as $property_name => $property_info){
    $options[$property_name] = $property_info['label'];
  }
  
  $form['default_properties'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => t('Default Properties')
  );
  
  //Now we need to set the defaults if the entity_type has any
  $properties = $entity_type->properties;
  
  $defaults = array();
  foreach($properties as $property => $info){
    $defaults[$property] = $property;
  }
  
  $form['default_properties']['#default_value'] = $defaults;
  // :S
  
  return $form;
}

function  eck__properties__form_default_properties_submit($form, &$state, $entity_type){
  
  $entity_type = $state['values']['entity_type'];
  $dp = $state['values']['default_properties'];
  foreach($dp as $property => $active){
    if($active){
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
    }else{
      $entity_type->removeProperty($property);
    }
  }
  
  $state['values']['entity_type'] = $entity_type;
}

function eck__properties__form_add_custom_properties($form, &$state, $entity_type){
  
  //When the add button is clicked, the submit callback directs the form to be rebuilt. 
   //Here we check to see if a new property was added, and keep it in a a list of new properties
   //to add them to the table on each rebuild 
   $new_properties = array();
   if(!empty($state['values']['property_name'])){
     if(!empty($state['values']['new_properties'])){
       $new_properties = $state['values']['new_properties'];
     }
     
     $new_properties[$state['values']['property_name']] =
     array(
       'label' => $state['values']['property_label'],
       'type' => $state['values']['property_type']
     );
     
     $form['new_properties'] = array('#type' => 'value', '#value' => $new_properties);
     
   }
  
  //This is the select table where all the new properties are shown.
  $header = array(
    'name' => t('Name'),
    'type' => t('Type'),
    'behavior' => t('Behaviour')
  );
    
  $options = array();
  
  $property_arrays = array('new_properties');
  
  foreach($property_arrays as $array_name){
    foreach(${$array_name} as $property_name => $property_info){
      $options[$property_name] = 
      array(
        'name' => $property_info['label'],
        'type' => $property_info['type'],
      );
      
      if(array_key_exists('behavior', $property_info)){
        $options[$property_name]['behavior'] = $property_info['behavior'];
      }else{
        $options[$property_name]['behavior'] = "";
      }
      
    }
  }
  
  $form["new_properties_table_label"] = array(
    '#markup' => '<h3>New properties</h3>'
  );
  
  $form['new_properties_table'] = 
  array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No other properties for this entity type.')
  );

  //Add new property
  $types[t('Generic')] = eck_property_types();
 
  $form["add_new_property"] = array(
    '#markup' => '<h3>Add new property</h3>'
  );

   $form['property_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
    '#required' => TRUE
  );

  $form["property_label"] = array(
    '#type' => 'textfield',
    '#title' => t("Name"),
    '#description' => t("A human readable name for the property."),
  );

  $form["property_name"] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('label'),
    ),
  );
  
  
  $form['property_add'] = array(
    '#type' => 'submit',
    '#value' => t('Add Property'),
  );
  
  return $form;
  
}

function eck__properties__form($form, &$state, $entity_type){
  //put the submit button first so it will be called by multiform;
  
  $form = eck__propeties__form_default_properties($form, $state, $entity_type);
  
  $form = eck__properties__form_add_custom_properties($form, $state, $entity_type);
  
  //CUSTOM PROPERTIES
  
  return $form;
}

function eck__properties__form_submit($form, &$state){
  if($state['values']['op'] == "Add Property"){
    $state['rebuild'] = TRUE;
  }
  else if($state['values']['op'] == "Save"){
    //Here we set up the entity type and save it.
    eck__properties__form_default_properties_submit($form, $state);
  }
}

function eck_get_default_properties(){
  return module_invoke_all('eck_default_properties');
}

function eck_get_default_property($name){
  $default_properties = eck_get_default_properties();
  return $default_properties[$name];
}

