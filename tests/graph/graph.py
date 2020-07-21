# matplotlib中有很多可用的模块，我们使用pyplot模块
import os
import matplotlib.pyplot as plt

def spent_time(case_name):
    current_dir = os.path.dirname(os.path.abspath(__file__))
    case_dir = os.path.join(current_dir, "case")
    times = list(range(1, 11))
    old = []
    new = []

    # rlp 编码次数
    first = True
    rlp_times = 0

    # 旧数据
    os.chdir(case_dir)
    os.system("platon-cpp -D OLD " + case_name + "_test.cpp")
    case_wasm_file = os.path.join(case_dir, case_name + "_test.wasm")
    command = "platon-test exec  --file " + case_wasm_file

    for i in range(len(times)):
        prco = os.popen(command)
        output_info = str(prco.read())
        lines = output_info.splitlines()
        for line in lines:
            print(line)
            if(line.startswith('spent time')):
                all = line.split(':')
                old.append(int(all[-1]))

            if(line.startswith('rlp encoding times') and first):
                all = line.split(':')
                rlp_times = int(all[-1])
                first = False

    # 新数据
    os.chdir(case_dir)
    os.system("platon-cpp " + case_name + "_test.cpp")
    command = "platon-test exec  --file " + case_wasm_file

    for i in range(len(times)):
        prco = os.popen(command)
        output_info = str(prco.read())
        lines = output_info.splitlines()
        for line in lines: 
            print(line)
            if(line.startswith('spent time')):
                all = line.split(':')
                new.append(int(all[-1]))
    
    # 生成图表
    os.chdir(current_dir)
    plt.plot(times, old, color='green', label='old')
    plt.plot(times, new, color='red', label='new')

    # 设置横坐标为times，纵坐标为population，标题为Population times correspondence
    plt.xlabel('times')
    plt.ylabel('elapsed time')
    title_info = case_name + ' rlp ' +  str(rlp_times) + ' times takes time'
    plt.title(title_info)
    plt.legend()

    # 设置纵坐标刻度
    plt.xticks(times)
    plt.yticks(list(range(0, 30)))

    # 保存图表
    plt.savefig(case_name + ".png")
    plt.close()


if __name__ == "__main__":
    try:
        spent_time("bigint_one")
        spent_time("bigint_four")
        spent_time("bigint_eight")
        spent_time("bigint_twelve")
        spent_time("bigint_sixteen")
        spent_time("string_one")
        spent_time("string_two")
        spent_time("string_three")
        spent_time("list_bigint_one")
        spent_time("list_bigint_two")
        spent_time("list_bigint_three")
        spent_time("list_string_one")
        spent_time("list_string_two")
        spent_time("list_string_three")

    except Exception as e:
        print('{} {}'.format('exception: ', e))
        print('generate spent time page file failure!!!')
        exit(1)
    
    else:
        print('{}{}'.format('\nSUCCESS\n', "generate spent time page file success!!!"))