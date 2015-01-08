@api @permissions
Feature: Bundle Permissions
  As a site administrator
  I want to control who can do what with entity types, bundles and entities
  so users don't get themselves in trouble.

  @setup
  Scenario Outline: This is a set up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities,Administer permissions" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with <type_label>
    And I fill in "edit-entity-type-name" with <type>
    And I fill in "edit-bundle-label" with <bundle_label>
    And I fill in "edit-bundle-name" with <bundle>
    And I check "Title"
    And I press the "Save" button
    And I visit <link>
    And I click "Manage fields"
    And I fill in "edit-fields-eck-add-extra-field-field-name" with "title"
    And I fill in "edit-fields-eck-add-extra-field-label" with "Title"
    And I fill in "edit-fields-eck-add-extra-field-widget-type" with "text"
    And I press the "Save" button
    And I visit <link>
    And I click <add_link>
    And I fill in "Title" with <entity_title>
    And I press the "Save" button

    Examples:
      | type_label | type      | bundle_label | bundle | entity_title   | link                                       | add_link  |
      | "Vehicle"  | "vehicle" | "Car"        | "car"  | "Toyota Prius" | "/admin/structure/entity-type/vehicle/car" | "Add Car" |
      | "Animal"   | "animal"  | "Dog"        | "dog"  | "Snoopy"       | "/admin/structure/entity-type/animal/dog"  | "Add Dog" |

  @bundle @drupal-perm
  Scenario: Users without the right permission can not access the bundle's overview page
    Given I am logged in as a user with the "Use the administration pages and help,List Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the text "Vehicle"
    And I should not see the link "Vehicle"

  @bundle @drupal-perm
  Scenario Outline: Users with the right permission can access the bundle's overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link <type_label>
    When I click <type_label>
    Then I should get a "200" HTTP response

    Examples:
      | type_label | permissions                                                                  |
      | "Vehicle"  | "Use the administration pages and help,List Entity Types,List Bundles"       |
      | "Animal"   | "Use the administration pages and help,List Entity Types,List Bundles"       |
      | "Vehicle"  | "Use the administration pages and help,List Entity Types,Administer Bundles" |
      | "Animal"   | "Use the administration pages and help,List Entity Types,Administer Bundles" |

  @bundle @eck-perm
  Scenario Outline: Users with the right permission can access the bundle's overview page (specific)
    Given I am logged in as a user with the "Use the administration pages and help,List Entity Types" permissions
    And the last user has the ECK permission <operation> "bundle" <object_id>
    And I visit "/admin/structure/entity-type"
    Then I should see the link "Vehicle"
    And I should not see the link "Animal"
    And I should see the text "Animal"
    When I click "Vehicle"
    Then I should get a "200" HTTP response

    Examples:
      | operation | object_id    |
      | "list"    | "vehicle\|*" |
      | "*"       | "vehicle\|*" |

  @bundle @drupal-perm
  Scenario: Users without the right permission can not add bundles from the overview page
    Given I am logged in as a user with the "List Bundles" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "Add bundle"

  @bundle @drupal-perm
  Scenario Outline: Users with the right permission can add bundles from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <link>
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response

    Examples:
      | link                                   | permissions                       |
      | "/admin/structure/entity-type/vehicle" | "List Bundles,Create Bundles"     |
      | "/admin/structure/entity-type/animal"  | "List Bundles,Create Bundles"     |
      | "/admin/structure/entity-type/vehicle" | "List Bundles,Administer Bundles" |
      | "/admin/structure/entity-type/animal"  | "List Bundles,Administer Bundles" |

  @bundle @eck-perm
  Scenario Outline: Users with the right permission can add bundles from the overview page (specific)
    Given I am logged in as a user with the "List Bundles" permissions
    And the last user has the ECK permission <operation> "bundle" <object_id>
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "Add bundle"

    Examples:
      | operation | object_id    |
      | "create"  | "vehicle\|*" |
      | "*"       | "vehicle\|*" |

  @bundle @drupal-perm
  Scenario: Users without the right permission can not delete bundles from the overview page
    Given I am logged in as a user with the "List Bundles" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "delete"

  @bundle @drupal-perm
  Scenario Outline: Users with the right permission can delete bundles from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <link>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples:
      | link                                   | permissions                       |
      | "/admin/structure/entity-type/vehicle" | "List Bundles,Delete Bundles"     |
      | "/admin/structure/entity-type/animal"  | "List Bundles,Delete Bundles"     |
      | "/admin/structure/entity-type/vehicle" | "List Bundles,Administer Bundles" |
      | "/admin/structure/entity-type/animal"  | "List Bundles,Administer Bundles" |

  @bundle @eck-perm
  Scenario Outline: Users with the right permission can delete bundles from the overview page (specific)
    Given I am logged in as a user with the "List Bundles" permissions
    And the last user has the ECK permission <operation> "bundle" <object_id>
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "delete"
     When I click "delete"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "delete"

    Examples:
      | operation | object_id    |
      | "delete"  | "vehicle\|*" |
      | "*"       | "vehicle\|*" |

  @cleanup
  Scenario Outline: This is a clean up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities" permissions
    Given I visit "/admin/structure/entity-type"
    And I click <type_label>
    And I click "Delete"
    And I press the "Delete" button

    Examples:
      | type_label |
      | "Vehicle"  |
      | "Animal"   |
