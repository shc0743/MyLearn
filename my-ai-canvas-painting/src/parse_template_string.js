/**
 * @param {string} str 
 * @param {object} variables 
 * @returns { string }
 */
function parse_template_string(str, variables) {
    // 使用DOMParser解析HTML
    const parser = new DOMParser();
    const doc = parser.parseFromString(str, "text/html");

    // 递归处理DOM节点
    const processNode = (node) => {
        // 处理元素节点
        if (node.nodeType === Node.ELEMENT_NODE) {
            // 检查v-if属性
            if (node.hasAttribute("v-if")) {
                const expression = node.getAttribute("v-if");
                let conditionValue;

                // 处理取反表达式（如!my_condition）
                if (expression.startsWith("!")) {
                    const varName = expression.slice(1);
                    conditionValue = !variables[varName];
                }
                // 处理普通变量
                else {
                    conditionValue = !!variables[expression]; // 转换为布尔值
                }

                // 根据条件值移除或保留节点
                if (!conditionValue) {
                    node.parentNode.removeChild(node);
                    return null; // 节点已被移除
                }

                // 移除已处理的v-if属性
                node.removeAttribute("v-if");
            }
            // 处理v-text属性
            if (node.hasAttribute("v-text")) {
                const varName = node.getAttribute("v-text");
                if (variables.hasOwnProperty(varName)) {
                    // 用变量值替换元素内容
                    node.textContent = variables[varName];
                }
                // 移除已处理的v-text属性
                node.removeAttribute("v-text");
            }

            // 递归处理子节点（使用反向遍历避免动态集合问题）
            const children = Array.from(node.childNodes);
            for (let i = children.length - 1; i >= 0; i--) {
                processNode(children[i]);
            }
        }
        return node;
    };

    // 处理template内容
    Array.from(doc.body.childNodes).forEach(processNode);

    // 转换为纯文本（保留原始换行和空格）
    return doc.body.innerText;
}
export { parse_template_string };