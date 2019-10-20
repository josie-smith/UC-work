# Calculates the join probability of an assignment given a Bayesian Network, or
# Belief Net.

def joint_prob(network, assignment):
    probability = 1;
    for key, value in assignment.items():
        parents = tuple(assignment[parent] for parent in network[key]['Parents'])
        if value :
            probability *= network[key]['CPT'][parents]
        else:
            probability *= (1 - network[key]['CPT'][parents])
    return probability


network = {
    'Burglary': {
        'Parents': [],
        'CPT': {
            (): 0.001
            }},

    'Earthquake': {
        'Parents': [],
        'CPT': {
            (): 0.002,
            }},
    'Alarm': {
        'Parents': ['Burglary','Earthquake'],
        'CPT': {
            (True,True): 0.95,
            (True,False): 0.94,
            (False,True): 0.29,
            (False,False): 0.001,
            }},

    'John': {
        'Parents': ['Alarm'],
        'CPT': {
            (True,): 0.9,
            (False,): 0.05,
            }},

    'Mary': {
        'Parents': ['Alarm'],
        'CPT': {
            (True,): 0.7,
            (False,): 0.01,
            }},
    }

p = joint_prob(network, {'John': True, 'Mary': True,
                         'Alarm': True, 'Burglary': False,
                         'Earthquake': False})
print("{:.8f}".format(p))
