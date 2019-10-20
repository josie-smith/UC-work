# kNN learning algorithm for machine learning, as well as functions to construct
# a perceptron then have it learnf from a set of training data.

import math


def euclidean_distance(v1, v2) :
    distances = []
    for i in range(len(v1)):
        distances.append(v1[i] - v2[i])
    return math.sqrt(sum(d**2 for d in distances))


def majority_element(labels) :
    maximum = 0
    answer = "NULL"
    for label in labels:
        count = labels.count(label)
        if count > maximum:
            maximum = count
            answer = label
        elif count == maximum:
            answer = min(label, answer)
    return answer


def knn_predict(input, examples, distance, combine, k) :
    distances = []
    for i in range(len(examples)):
        value, output = examples[i]
        distances.append([value, output, distance(value, input)])
    knn = []
    while len(knn) <k :
        next = min(distances, key=lambda x: x[2])
        distances.remove(next)
        knn.append(next)
    while len(distances) > 0 and knn[-1][2] == min(distances, key=lambda x: x[2])[2]:
        next = min(distances, key=lambda x: x[2])
        distances.remove(next)
        knn.append(next)
    return combine([item[1] for item in knn])


def construct_perceptron(weights, bias):
    """Returns a perceptron function using the given paramers."""
    def perceptron(input):
        total = bias
        for i in range(len(input)):
            total += weights[i] * input[i]
        return 1 if total >= 0 else 0
    return perceptron


def accuracy(classifier, inputs, expected_outputs) :
    flags = []
    for i in range(len(inputs)):
        flags.append(0 if classifier(inputs[i]) - expected_outputs[i] != 0 else 1)
    return sum(flags)/len(flags)


def learn_perceptron_parameters(weights, bias, training_examples, learning_rate, max_epochs) :
    for epoch in range(max_epochs):
        changed = False
        for example in training_examples:
            perceptron = construct_perceptron(weights, bias)
            y = perceptron(example[0])
            for i in range(len(weights)):
                new_weight = weights[i] + learning_rate * example[0][i] * (example[1] - y)
                if new_weight != weights[i]:
                    changed = True
                    weights[i] = new_weight
            new_bias = bias + learning_rate * (example[1] - y)
            if new_bias != bias:
                    changed = True
                    bias = new_bias
        if not changed:
            break
    return weights, bias



weights = [2, -4]
bias = 0
learning_rate = 0.5
examples = [
  ((0, 0), 0),
  ((0, 1), 1),
  ((1, 0), 1),
  ((1, 1), 0),
  ]
max_epochs = 50

weights, bias = learn_perceptron_parameters(weights, bias, examples, learning_rate, 50)
print(f"Weights: {weights}")
print(f"Bias: {bias}\n")
