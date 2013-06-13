<?php

function eck__properties__form($form, &$state, $entity_type_name){
  if (!isset($entity_type_name)) {
    $entity_type = new EntityType();
  }
  else {
    $entity_type = EntityType::loadByName($entity_type_name);
  }
  
  $form['entity_type'] = array('#type' => 'value', '#value' => $entity_type);
  
  $form['info'] = array('#markup' => "<p><strong>HELP:</strong> The add property button will temporarily add a property to the
    entity type, to finalize adding the property, click the checkbox on the left and click save </p>");
  
  //When the add button is clicked, the submit callback directs the form to be rebuilt. 
   //Here we check to see if a new property was added, and keep it in a a list of new properties
   //to add them to the table on each rebuild 
   $entity_type_properties = $entity_type->properties;
   
   if(!array_key_exists('values', $state) || !array_key_exists('new_properties', $state['values'])){
     $new_properties = eck_get_default_properties();
     $new_properties = array_merge($new_properties, $entity_type_properties);
   }else{
     $new_properties = $state['values']['new_properties'];
   }
  
   if(!empty($state['values']['property_name'])){
     $property_name = $state['values']['property_name'];
     $property_type = $state['values']['property_type'];
     //OK we need to do a little bit of filtering do the oddity of modifying existing
     //properties through the add Property form #fail @todo need to get a better interface.
     //But until then, we need to keep the form from changing the property_type or property_name of the already
     //existing properties
     
     if(!array_key_exists($property_name, $new_properties) || 
       $new_properties[$property_name]['type'] == $property_type){
     
       $new_properties[$property_name] =
       array(
         'label' => $state['values']['property_label'],
         'type' => $state['values']['property_type']
       );
       
       if(array_key_exists('property_behavior', $state['values'])){
         $new_properties[$state['values']['property_name']]['behavior'] = 
           $state['values']['property_behavior'];
       }
     }else{
       drupal_set_message("Can not modify the type of {$property_name}", "error");
     }
   }
   
   $form['new_properties'] = array('#type' => 'value', '#value' => $new_properties);
  
  //This is the select table where all the new properties are shown.
  $header = array(
    'machine_name' => t('Machine Name'),
    'name' => t('Name'),
    'type' => t('Type'),
    'behavior' => t('Behaviour')
  );
  
  $options = array();
  
  //@todo can we do this in a better way.. the only way I can think of is, load all defaults,
  //and then load all the properties in the entity type, while modifying the before added default
  //properties given the entity type settings
  
  $property_arrays = array('new_properties');
  
  foreach($property_arrays as $array_name){
    foreach(${$array_name} as $property_name => $property_info){
      $options[$property_name] = 
      array(
        'machine_name' => $property_name,
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
    '#markup' => '<h3>Properties</h3>'
  );
  
  $defaults = array();
  foreach($entity_type_properties as $property => $info){
    $defaults[$property] = 1;
  }
  
  $form['new_properties_table'] = 
  array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No other properties for this entity type.'),
    '#default_value' => $defaults
  );
  
  return $form;
}

function eck__add_property__form($form, &$state, $entity_type_name){
  
  $form['entity_type_name'] = array(
    '#type' => 'value',
    '#value' => $entity_type_name
  );
  
  //Add new property
  $property_types = eck_get_property_types();
  
  //lets transform the property_types info array into something appropriate for
  //a select list
  $property_type_options = array();
  foreach($property_types as $property_name => $info){
    $property_type_options[$property_name] = $info['label'];
  }
  
  $types[t('Generic')] = $property_type_options;
 
  $form["add_new_property"] = array(
    '#markup' => '<h3>Add new property</h3>'
  );

   $form['property_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
    '#required' => TRUE,
    //'#after_build' => array('_eck_deactivate_on_save'),
    /*'#ajax' => array(
      'callback' => 'eck_properties_property_config',
      'wrapper' => 'eck-property-config',
      'method' => "replace"
    )*/
  );
   
 $form['property_select_type'] = array(
    '#type' => 'submit',
    '#value' => t('Select Property Type'),
  );
 
  if(array_key_exists('values', $state) && !empty($state['values']['property_type'])) {
    
    $property_type = $state['values']['property_type'];
    $property_type_info = eck_get_property_types($property_type);

    $default_schema = $property_type_info['class']::schema();

    $form['default_schema'] = array(
      '#type' => 'value',
      '#value' => $default_schema
    );

    $sf = new SchemaForm($default_schema);

    $schema_form = $sf->getSchemaForm();
    foreach($schema_form as $name => $field){
      unset($schema_form[$name]);
      $schema_form["schema_{$name}"] = $field;
    }

    $form['schema'] = $schema_form;
    
    $form["property_label"] = array(
      '#type' => 'textfield',
      '#title' => t("Name"),
      '#description' => t("A human readable name for the property."),
      '#required' => TRUE,
      '#after_build' => array('_eck_deactivate_on_save')
    );

    $form["property_name"] = array(
      '#type' => 'machine_name',
      '#machine_name' => array(
        'exists' => '_eck_fake_exists',
        'source' => array('property_label'),
      ),
     '#after_build' => array('_eck_deactivate_on_save')
    );

    $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
    $options = array();

    foreach($behavior_plugins as $behavior => $info){
      $options[$behavior] = $info['label'];
    }

    $form['property_behavior'] = array(
      '#type' => 'select',
      '#title' => t('Behavior'),
      '#options' => array('' => t('- Please choose -')) + $options
    );


    $form['property_add'] = array(
      '#type' => 'submit',
      '#value' => t('Add Property'),
    );

  }
  
  return $form;
}

function eck__add_property__form_submit($form, &$state){
  $values = $state['values'];
  
  if(strcmp($values['op'], t('Select Property Type')) == 0){
    $state['rebuild'] = TRUE;
  }else{
    //lets save schema customization
    $default_schema = $values['default_schema'];
    unset($values['default_schema']);

    $new_schema = array();
    //lets get the values from the form
    foreach($values as $name => $value){
      if(substr_count($name, "schema") > 0){
        $new_name = str_replace("schema_", "", $name);
        if(strcmp($new_name, "unsigned") == 0 || strcmp($new_name, "not null") == 0){
          $value = ($value)?TRUE:FALSE;
        }
        $new_schema[$new_name] = $value;
      }
    }

    //$diff = array_diff($new_schema, $default_schema);

    $entity_type_name = $values['entity_type_name'];
    $entity_type = EntityType::loadByName($entity_type_name);

    $entity_type->addProperty($values['property_name'], $values['property_label'], 
            $values['property_type'], $values['property_behavior'], $new_schema);

    $entity_type->save();
    eck_get_property_types(NULL, TRUE);
  }
}

function eck_properties_property_config($form, &$state){
  return $form['eck_property_config2'];
}

function eck__properties__form_validate($form, &$state){
  if($state['values']['op'] == t('Add Property')){
    if (isset($state['values']['property_behavior'])) {
      $property_behavior = $state['values']['property_behavior'];
      $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
      if (isset($behavior_plugins[$property_behavior]['unique']) && $behavior_plugins[$property_behavior]['unique']) {
        foreach ($state['values']['new_properties'] as $property_name => $property_info) {
          if ($property_info['behavior'] == $property_behavior) {
            form_set_error('property_behavior', t('There can be only one property with \'@name\' behavior per entity.',
              array('@name' => $behavior_plugins[$property_behavior]['label'])));
          }
        }
      }
    }
  }
}

function eck__properties__form_submit($form, &$state){
  if($state['values']['op'] == t('Add Property')) {
    $state['rebuild'] = TRUE;
  }
  else if($state['values']['op'] == t('Save')) {
    //Here we want to add the properties to the entity type and save it
    $entity_type = $state['values']['entity_type'];
    //dpm($entity_type, "Entity Type");
    
    foreach($state['values']['new_properties_table'] as $property => $active){
      if($active){
        $info = $state['values']['new_properties'][$property];
        if(array_key_exists('behavior', $info)){
          $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
        }else{
          $entity_type->addProperty($property, $info['label'], $info['type']);
        }
      }else{
        $entity_type->removeProperty($property);
      }
    }
    
    $entity_type->save();
    
    //lets flush the cache so new behaviors and properties will get set up correctly
    drupal_flush_all_caches();
  }
}

function _eck_deactivate_on_save($element, &$state){
  if(array_key_exists('input', $state) && array_key_exists('op', $state['input']) && 
    $state['input']['op'] == t('Save')) {
    isset($element['#element_validate']) ? $element['#element_validate'] = NULL : NULL;
    isset($element['#needs_validation']) ? $element['#needs_validation'] = NULL : NULL;
  }
  
  return $element;
}