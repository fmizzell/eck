<?php

// Only allow entering the desired bundle name when creating a new entity.
/*if ($entity_type->is_new) {
    
$form['bundle_label'] = array(
  '#type' => 'textfield',
  '#title' => 'Bundle (optional)',
  '#description' => 'A bundle with the same name as the entity type is created by default, this will override the default',
);

$form['bundle_name'] = array(
  '#type' => 'machine_name',
  '#required' => FALSE,
  '#machine_name' => array(
    'exists' => '_eck_fake_exists',
    'source' => array('bundle_label'),
  ),
);
}*/

//$form = eck__default_properties__form($form, $state, $entity_type);
//$form['#validate'][] = 'eck__entity_type__form_validate';

//Add the bundle to the table
//Process the bundle input from the user
/*if (!empty($state['values']['bundle_name'])) {
  $bundle_name = $state['values']['bundle_name'];
  if (!empty($state['values']['bundle_label'])) {
    $bundle_label = $state['values']['bundle_label'];
  }
  else {
    $bundle_label = ucfirst($bundle_name);
  }
}
else {
  $bundle_name = $entity_type_name;
  $bundle_label = $entity_type_label;
}

//Let's set up the object and save it to the db.
$bundle = new Bundle();
$bundle->entity_type = $entity_type->name;
$bundle->name = $bundle_name;
$bundle->label = $bundle_label;
$bundle->save();
Bundle::loadAll(NULL, TRUE);
}

//lets handle the default properties
eck__default_properties__form_submit($form, $state, $entity_type);*/

//Clear info caches in order to pick up newly created entities.
/*EntityType::loadAll(NULL, TRUE);
drupal_get_schema(NULL, TRUE);
entity_info_cache_clear();
variable_set('menu_rebuild_needed', TRUE);
if($entity_type->is_new){
  drupal_set_message(t('Entity type %entity_type has been created.', array('%entity_type' => $entity_type->label)));
} else {
  drupal_set_message(t('Entity type %entity_type has been updated.', array('%entity_type' => $entity_type->label)));
}*/