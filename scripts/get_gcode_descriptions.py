#!/usr/bin/env python3
import yaml
import re
import sys
import requests
from bs4 import BeautifulSoup
from pathlib import Path

def fetch_headings(url):
    response = requests.get(url)
    response.raise_for_status()  # Raise an exception for bad status codes
    soup = BeautifulSoup(response.text, 'html.parser')
    return soup.find_all(['h1', 'h2'])
    

def parse_code_descriptions(headings):
    descriptions = {}
    
    # Find all G-commands and M-commands sections
    for heading in headings:
        heading_id = heading.get('id', None)
        if heading_id == None:
            continue

        if match := re.match(r'([gmtGMT])(\d+(\.\d+)?)', heading_id):
            command_type = match.group(1)
            command_num = match.group(2)
            description = heading.next.next_sibling.split(': ')[1]
            descriptions[f'gcode_{command_type.upper()}{command_num}_desc'] = description

    return descriptions

def update_yaml_file(yaml_file_path, descriptions):
    # Read existing YAML
    with open(yaml_file_path, 'r', encoding='utf-8') as f:
        yaml_content = yaml.safe_load(f)
    
    # Update descriptions
    if 'en-GB' in yaml_content:
        for key, value in descriptions.items():
            yaml_content['en-GB'][key] = value
    
    # Write updated YAML
    with open(yaml_file_path, 'w', encoding='utf-8') as f:
        yaml.dump(yaml_content, f, allow_unicode=True, default_flow_style=False)

def main(yaml_file_path):
    # Fetch webpage content
    url = "https://docs.duet3d.com/en/User_manual/Reference/Gcodes"
    try:
        webpage_content = fetch_headings(url)
    except requests.RequestException as e:
        print(f"Error fetching webpage: {e}", file=sys.stderr)
        sys.exit(1)
    
    # Extract descriptions
    descriptions = parse_code_descriptions(webpage_content)
    
    # Update YAML file
    update_yaml_file(yaml_file_path, descriptions)

if __name__ == "__main__":
    yaml_file = Path(__file__).parent.parent / 'i18n' / 'en-GB.yml'
    main(yaml_file)
