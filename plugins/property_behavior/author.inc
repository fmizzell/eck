<?php
// Plugin definition
$plugin = array(
  'label' => "Author",
  'hook_entity_save' => 'eck_author_property_entity_save'
);