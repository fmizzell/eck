<?php
use ECK\Core\EntityType;

/**
 * Callback for adding entity types functionality.
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 * @param $entity_type
 *   (EntityType) an EntityType object
 */
function eck__entity_type__edit_form($form, &$state, $entity_type = NULL) {
  
  $fb = new ECK\UI\Widgets\FormBuilder($entity_type);
  
  $widget = new ECK\UI\Widgets\Text();
  $widget->setLabel("Name");
  $widget->setDescription(t('A human readable name for the entity type'));
  $fb->addWidget("name", $widget);
  
  $widget = new ECK\UI\Widgets\Text();
  $widget->setLabel("Label");
  $fb->addWidget("label", $widget);
  $form = $fb->build($form);
  
  $form['form_builder'] = array(
    '#type' => 'value',
    '#value' => $fb,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for adding an entity type.
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 */
function eck__entity_type__edit_form_submit($form, &$state) {
  if($state['values']['op'] == t("Save")){
    $fb = $state['values']['form_builder'];
    $fb->update($state['values']);
  }
}

function eck__entity_type__delete_form($form, &$state, $entity_type){
  $form['entity_type'] = 
  array(
  '#type' => 'value',
  '#value' => $entity_type
  );

  $message = t("Are you sure that you want to delete the entity type '{$entity_type->getName()}'");

  $caption = t("All of the data from this entity type 
  will be deleted. This action cannot be undone.");

  return 
  confirm_form($form, $message, 
  NULL, $caption, t('Delete'));
}

function eck__entity_type__delete_form_submit($form, &$state){
  $entity_type = $state['values']['entity_type'];
  $entity_type->delete();
}
