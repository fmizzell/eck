<?php
/**
 * Implements hook_ctools_plugin_directory() to let the system know
 * where our property_behavior plugins are.
 */
function eck_ctools_plugin_directory($owner, $plugin_type) {
  if ($owner == 'eck' && $plugin_type == 'property_behavior') {
    return 'plugins/' . $plugin_type;
  }
}

/**
 * Implements hook_ctools_plugin_type() to inform the plugin system that Page
 * Manager owns task, task_handler, and page_wizard plugin types.
 *
 * All of these are empty because the defaults all work.
 */
function eck_ctools_plugin_type() {
  return array(
    'property_behavior' => array(),
  );
}

function eck_entity_info_alter(&$info){
  
  foreach (EntityType::loadAll() as $entity_type) {
    $entity_type_info = $info[$entity_type->name];
    
    $entity_type_info =
    eck_property_behavior_invoke_plugin_alter($entity_type, 'entity_info',$entity_type_info);
    
    if($entity_type_info){
      $info[$entity_type->name] = $entity_type_info;
    }
  }
}


/**
 * This function checks whether a property is implementing any of the supported
 * property behavior functions
 * 
 * @todo I think this function could simplify the code of eck_property_behavior_invoke_plugin and alter
 */
function eck_property_behavior_implements($entity_type, $property, $function_name){
  $properties = $entity_type->properties;
  $info = $entity_type->properties[$property];
  $return = FALSE;
  
  if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
     $behavior = $info['behavior'];
     $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
     $function = ctools_plugin_get_function($plugin, $function_name);
     
     if($function){
       $return = $function;
     }
  }
  
  return $return;
}

/**
 * A general function to call specific functions in a property behavior plugin
 */
function eck_property_behavior_invoke_plugin($entity_type, $function_name, $all = array(), 
  $specific = array()){
  
  $properties = $entity_type->properties;
  $returns = array();
  
  foreach($properties as $property => $info){
    //If there is a behavior associated with this property we need to call the appropiate hooks
    if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
      $behavior = $info['behavior'];
     
      $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
      
      $function = ctools_plugin_get_function($plugin, $function_name);
  
      if($function){
        if(array_key_exists($property, $specific)){
          $return = $function($property, array_merge($all, $specific[$property]));
        }else{
          $return = $function($property, $all);
        }
        
        if($return){
          $returns[$property] = $return;
        }
      }
    }
  }
  
  return $returns;
}
 
function eck_property_behavior_invoke_plugin_alter($entity_type, $function_name, $var){
  
  $properties = $entity_type->properties;
  $return = $var;
  
  foreach($properties as $property => $info){
    //If there is a behavior associated with this property we need to call the appropiate hooks
    if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
      $behavior = $info['behavior'];
     
      $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
      
      $function = ctools_plugin_get_function($plugin, $function_name);
  
      if($function){
        if(array_key_exists('behavior_config', $info)){
          $return['config'] = $info['behavior_config'];
        }
        $return = $function($property, $return);
      }
    }
  }
  
  return $return;
  
}

function eck_property_behavior_setter(&$data, $name, $value, $langcode, $type, $info){
  $entity = $data;
  $entity_type_name = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type_name);
  $property = $name;
  
  $function = eck_property_behavior_implements($entity_type, $property, 'setter');
  
  if($function){
    return $function($property, array('entity'=> $entity, 'value' => $value));
  }
}

function eck_property_behavior_getter($data, array $options, $name, $type, $info){
  $entity = $data;
  $entity_type_name = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type_name);
  $property = $name;
  
  $function = eck_property_behavior_implements($entity_type, $property, 'getter');
  
  if($function){
    return $function($property, array('entity'=> $entity));
  }
}

function eck_property_behavior_validation($value, $info){
  $entity = $info['parent']->value();
  $entity_type_name = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type_name);
  $property = $info['schema field'];
  
  $function = eck_property_behavior_implements($entity_type, $property, 'validation');
  
  if($function){
    return $function($property, array('value' => $value));
  }
}

function eck__property_behavior__form($form, &$state, $entity_type_name, $property){
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;
  if(array_key_exists($property, $properties)){
    $form['entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity_type
    );

    $form['property'] = array(
      '#type' => 'value',
      '#value' => $property
    );

    ctools_include("plugins");
    $plugins = ctools_get_plugins("eck", "property_behavior");
    $behaviors = array('_ NONE _');
    $behaviors = array_merge($behaviors, array_keys($plugins));

    $form['behaviors'] = array(
      '#type' => 'value',
      '#value' => $behaviors
    );

    $key = array_search($properties[$property]['behavior'], $behaviors);

    //we can have 3 possible states,
    //1) there is no behavior for the property and non has been selected for
    //configuration
    //2) there is no behavior for the property, but one has been chosen to be
    //configured
    //3) This property has a behavior already

    //has a behavior been selected?
    $behavior = FALSE;
    if(array_key_exists('values', $state)){
      $values = $state['values'];
      $behavior = $values['behavior'];
    }

    if(!$key && !$behavior){
      $form['behavior'] = array(
        '#title' => 'behavior',
        '#type' => 'select',
        '#options' => $behaviors,
        '#default_value' => $key
      );

      $form['configure'] = array(
        '#type' => 'submit',
        '#weight' => 10000,
        '#value' => t('Configure'),
      );
    }
    else if(!$key && $behavior){
      $form['behavior'] = array(
        '#title' => 'behavior',
        '#type' => 'select',
        '#options' => $behaviors,
        '#default_value' => $key
      );

      //need to get the behaviors configuration form
      $behavior_name = $behaviors[$behavior];
      $function = ctools_plugin_get_function($plugins[$behavior_name], "config_form");
      if($function){
        $config_form = $function();
        foreach($config_form as $key => $element){
          unset($config_form[$key]);
          $config_form["config_{$key}"] = $element;
        }
      }
      
      $form['config_form_wrapper'] = array('#type' => 'fieldset', '#title' => "Configure Behavior");
      $form['config_form_wrapper']['config_form'] = $config_form;

      $form['submit'] = array(
        '#type' => 'submit',
        '#weight' => 10000,
        '#value' => t('Save'),
      );
    }
    else{
      $form['behavior'] = array(
        '#markup' => "<h3>{$behaviors[$key]}</h3>"
      );

      $form['remove'] = array(
        '#type' => 'submit',
        '#weight' => 10000,
        '#value' => t('Remove'),
      );
    }

  }else{
    drupal_set_message('This property does not exists');
    drupal_goto(eck__entity_type__path()."/{$entity_type->name}/property");
  }

  return $form;
}

function eck_property_behavior_get_config_values($values){
  $config = array();
  foreach($values as $key => $value){
    if(substr_count($key, "config_") > 0){
      $new_key = str_replace("config_", "", $key);
      $config[$new_key] = $value;
    }
  }
  
  return $config;
}
function eck__property_behavior__form_submit($form, &$state){

  if($state['values']['op'] == "Configure"){
    $state['rebuild'] = TRUE;
  }else{
    $values = $state['values'];
    
    $behavior_config = eck_property_behavior_get_config_values($values);
    
    $entity_type = $values['entity_type'];
    $property = $values['property'];
    $behaviors = $values['behaviors'];
    $behavior = array_key_exists('behavior', $values)?$values['behavior']:FALSE;

    $properties = $entity_type->properties;

    if($behavior != 0){
      $properties[$property]['behavior'] = $behaviors[$behavior];
      if(!empty($behavior_config)){
        $properties[$property]['behavior_config'] = $behavior_config;
      }
      $entity_type->properties = $properties;
    }else{
      $properties[$property]['behavior'] = NULL;
      $entity_type->properties = $properties;
    }

    $entity_type->save();

    EntityType::loadAll(NULL, TRUE);

    $state['redirect'] = eck__entity_type__path()."/{$entity_type->name}/property";
  }
}

//Entity Hooks
function eck_entity_presave($entity, $entity_type){
  $entity_type = EntityType::loadByName($entity_type);
  
  //this is an eck entity
  if($entity_type){
    eck_property_behavior_invoke_plugin($entity_type, 'entity_save', 
      array('entity' => $entity));
  }
}

function eck_entity_update($entity, $entity_type){
  $entity_type = EntityType::loadByName($entity_type);
  
  //this is an eck entity
  if($entity_type){
    eck_property_behavior_invoke_plugin($entity_type, 'entity_update', 
      array('entity' => $entity));
  }
}

function eck_entity_delete($entity, $entity_type){
  $entity_type = EntityType::loadByName($entity_type);
  
  //this is an eck entity
  if($entity_type){
    eck_property_behavior_invoke_plugin($entity_type, 'entity_delete', 
      array('entity' => $entity));
  }
}

function eck_entity_view_alter(&$view){
  $entity_types = EntityType::loadAll();
  
  $this_entity_type = $view['#entity_type'];
  
  foreach($entity_types as $et){
    if($et->name == $this_entity_type){
      $entity = $view['#entity'];
      $this_bundle = $entity->type;
      //lets add contextual links to our entities
      //In eck you can change the paths of any of the possible operations, 
      //since contextual links are dependent on the hierarchy of those paths,
      //changing the paths could cause of contextual links not to work correctly
      
      $view['#contextual_links']['eck'] =
      array(
        "{$this_entity_type}/{$this_bundle}",array($entity->id));
    }
  }
}