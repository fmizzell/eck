<?php

function eck__property__edit_form($form, &$state, $entity_type){
  
  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type
  );
  
  //Add new property
  module_load_include('php', 'eck', 'core/property_type/helper');
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

    $sf = new ECK\PropertyTypes\SchemaForm($default_schema);

    $schema_form = $sf->getSchemaForm();
    foreach($schema_form as $name => $field){
      unset($schema_form[$name]);
      $schema_form["schema_{$name}"] = $field;
    }

    $form['schema'] = array('#type' => 'fieldset', '#collapsible' => TRUE, '#collapsed' => TRUE, '#title' => 'Schema Configuration');
    $form['schema']['form'] = $schema_form;
    
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

    $form['property_add'] = array(
      '#type' => 'submit',
      '#value' => t('Add Property'),
    );

  }
  
  return $form;
}

function eck__property__edit_form_submit($form, &$state){
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
    
    //if the default is empty lets take it off
    if(array_key_exists('default', $new_schema) && empty($new_schema['default'])){
      unset($new_schema['default']);
    }
    //lets format the not null key to work with schema
    if(array_key_exists('not_null', $new_schema)){
      $temp = $new_schema['not_null'];
      unset($new_schema['not_null']);
      $new_schema['not null'] = $temp;
    }

    $entity_type = $values['entity_type'];
    
    $entity_type->addProperty($values['property_name'], $values['property_type']);
    $entity_type->getProperty($values['property_name'])->setLabel($values['property_label']);

    $entity_type->save();
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
