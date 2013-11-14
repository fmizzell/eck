<?php
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