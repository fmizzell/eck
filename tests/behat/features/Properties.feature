@api @properties
Feature: Properties
  As a content architect
  I want to be able to forge my entities with custom attributes (properties)
  so my content will do exactly what it needs to do

  Background:
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities,Update Properties" permissions

  @setup
  Scenario: Setting up for the tests
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with "Vehicle"
    And I fill in "edit-entity-type-name" with "vehicle"
    And I fill in "edit-bundle-label" with "Car"
    And I fill in "edit-bundle-name" with "car"
    # And I check "Title"
    And I press the "Save" button

  @property
  Scenario Outline: I should be able to create a property set a value and then delete it
    # Add the property
    Given I visit "/admin/structure/entity-type/vehicle/property"
    And I click "Add property"
    And I fill in "edit-property-type" with <type>
    And I press the "Select Property Type" button
    And I fill in "edit-property-label" with <label>
    And I fill in "edit-property-name" with <name>
    And I press the "Add Property" button
    When I visit "/admin/structure/entity-type/vehicle/property"
    Then I should see the text <label>

    # Configure the widget
    Given I visit "/admin/structure/entity-type/vehicle/car/fields"
    And I fill in "edit-fields-eck-add-extra-field-label" with <label>
    And I fill in "edit-fields-eck-add-extra-field-field-name" with <name>
    And I fill in "edit-fields-eck-add-extra-field-widget-type" with "text"
    And I press the "Save" button
    Then I should see the text <label>

    # Create a test entity
    Given I visit "/admin/structure/entity-type/vehicle/car"
    And I click "Add Car"
    And I fill in <id> with <value>
    And I press the "Save" button
    Then I should see the text "has been saved"

    # Confirm the value was saved
    Given I visit "admin/structure/entity-type/vehicle/car"
    And I click "edit"
    Then the <id> field should contain <value>

    # Clean up. Delete the entity
    Given I visit "admin/structure/entity-type/vehicle/car"
    And I click "delete"
    Then I press the "Delete" button

    # @todo: add behavior tests
    # @todo: delete individual properties

  Examples:
    | type               | label | name  | id         | value                                  |
    | "text"             | "T"   | "t"   | "edit-t"   | "Toyota Prius"                         |
    | "integer"          | "I"   | "i"   | "edit-i"   | "-123456"                              |
    | "decimal"          | "D"   | "d"   | "edit-d"   | "45.98"                                |
    | "positive_integer" | "PI"  | "pi"  | "edit-pi"  | "987"                                  |
    | "fixed_size_text"  | "FST" | "fst" | "edit-fst" | "Toyota Prius"                         |
    | "long_text"        | "LT"  | "lt"  | "edit-lt"  | "Toyota Prius"                         |
    | "blob"             | "B"   | "b"   | "edit-b"   | "Toyota Prius"                         |
    | "datetime"         | "DT"  | "dt"  | "edit-dt"  | "1420775194"                           |
    | "language"         | "L"   | "l"   | "edit-l"   | "en"                                   |
    | "uuid"             | "U"   | "u"   | "edit-u"   | "45c91bc8-97b2-11e4-b100-123b93f75cba" |

  @cleanup
  Scenario: This is a clean up step
    Given I visit "/admin/structure/entity-type"
    And I click "Vehicle"
    And I click "Delete"
    And I press the "Delete" button
